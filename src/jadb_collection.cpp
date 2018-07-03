#include "jadb_collection.h"
#include "jadb_logger.h"
#include "jadb_database.h"
#include "jadb_filesystem.h"
#include "jadb_syncronizer.h"
#include "jadb_stats.h"
#include <memory>
#include <vector>

#define MAX_RECORD_LENGTH (512 * 1024)

#undef GetObject

using namespace jadb;

Collection::Collection(std::string name, boost::filesystem::path path, class Database* db)
    : m_name(name), m_path(path), m_dataDir(path), m_indicesDir(path), m_db(db)
{
    m_dataDir.append("data");
    m_indicesDir.append("idx");
    m_path.append(name);

    boost::filesystem::create_directories(m_dataDir);
    boost::filesystem::create_directories(m_indicesDir);
    
    std::vector<std::string> data;
    std::vector<std::string> indices;

    auto file = FileSystem::Get(m_path);
    file->close();
    file->open(std::ios::in | std::ios::binary);
    file->lock();
    uint32_t nextId = 0;
    Serialization(file).deserialize(nextId);
    m_ids.store(nextId);
    ArraySerialization(file).deserialize<std::string>(indices);
    ArraySerialization(file).deserialize<std::string>(data);

    for (auto& i : indices)
    {
        auto p = m_indicesDir;
        p.append(i);
        Logger::msg(1) << "Loaded index file " << p.generic_string() << " for collection " << m_name;
        m_indices.insert(std::make_pair(p.generic_string(), std::make_shared<IndexFile>(p)));
    }

    for (auto& i : data)
    {
        auto file = std::make_shared<DataFile>(i, this);

        Logger::msg(1) << "Loaded data file " << i << " for collection " << m_name << " (" << file->header().rows() << " rows";
        m_data.insert(std::make_pair(i, file));
    }
    file->unlock();

    Syncronizer::SyncronizationTask task([&]() {
        Logger::msg(2) << "Syncronizing collection " << m_name;
        std::lock_guard<std::recursive_mutex> guard(m_mx);
        std::vector<std::string> data;
        std::vector<std::string> indices;
        std::for_each(m_indices.begin(), m_indices.end(), [&](auto& p) {
            indices.push_back(p.second->name());
        });

        data.reserve(m_data.size());
        std::for_each(m_data.begin(), m_data.end(), [&](auto& p) {
            data.push_back(p.first);
        });

        auto file = FileSystem::Get(m_path);
        file->lock();
        file->open(std::ios::out | std::ios::binary | std::ios::trunc);

        Serialization(file).serialize(m_ids.load());
        ArraySerialization(file).serialize<std::string>(indices);
        ArraySerialization(file).serialize<std::string>(data);
        file->flush();
        file->close();
        file->unlock();
        Logger::msg(2) << "Syncronization of collection " << m_name << " done";
    });

    Syncronizer::addOperation(task);
}

Collection::~Collection()
{
}

uint32_t Collection::recordsPerFile() const
{
    return m_mapper.buckets();
}

void Collection::createIndex(std::string name, std::vector<std::string> &fields)
{
    OperationDuration dur(Statistics::Type::IndexCreation);
    if (m_indices.find(name) != m_indices.end())
    {
        return;
    }

    auto path = indexFilePath(name);
    auto index = std::make_shared<IndexFile>(path, name, fields);
    m_indices.insert(std::make_pair(path.generic_string(), index));
    for (auto& dt : m_data)
    {
        auto b = dt.second->begin();
        auto e = dt.second->end();
        while (b != e)
        {
            if(dt.second->checkSignature(Record::RecordSignature, b.absolutePos()))
                index->add(*b);
            ++b;
        }
    }
}

std::vector<Record> Collection::searchByIndex(std::string index, std::unordered_map<std::string, std::string>& filter, size_t limit, size_t skip)
{
    OperationDuration dur(Statistics::Type::SearchByIndex);
    std::vector<Record> res;
    auto idx = m_indices.find(indexFilePath(index).generic_string());
    if (idx == m_indices.end())
        return res;

    nlohmann::json query;
    for (auto q : filter)
    {
        query.emplace(q.first, q.second);
    }
    auto ids = idx->second->get(query, skip, limit);
    for (auto id : ids)
    {
        res.emplace_back(get(id));
    }
    return res;
}

std::vector<Record> Collection::query(const Query& query)
{
    std::vector<Record> res;
    std::set<uint64_t> *filter = nullptr;
    for (auto& file : m_data)
    {
        if (!query.exec(*(file.second), filter))
        {
            return std::vector<Record>();
        }
    }
    return res;
}

std::shared_ptr<DataFile> Collection::bucket(uint32_t bucket, bool create)
{
    auto path = m_dataDir;
    path.append(std::to_string(bucket));
    auto fileIt = m_data.find(path.generic_string());
    if (fileIt == m_data.end())
    {
        if (create)
        {
            m_data.insert(std::make_pair(path.generic_string(), std::make_shared<DataFile>(path, this)));
        }
        else
        {
            return nullptr;
        }
    }

    return m_data.find(path.generic_string())->second;
}

void Collection::removeBucket(uint32_t num)
{
    auto path = m_dataDir;
    path.append(std::to_string(num));
    m_data.erase(path.generic_string());
}

boost::filesystem::path Collection::indexFilePath(const std::string& name) const
{
    auto path = m_indicesDir;
    path.append(name);
    return path;
}

std::string Collection::name() const
{
    return m_name;
}

void Collection::lock()
{
    m_mx.lock();
}

void Collection::unlock()
{
    m_mx.unlock();
}

bool Collection::hasIndex(const std::string& name) const
{
    return m_indices.find(indexFilePath(name).generic_string()) != m_indices.end();
}

uint64_t Collection::insert(Record& record)
{
    OperationDuration dur(Statistics::Type::Insert);
    if (record.id() == 0)
        record.setId(m_ids.fetch_add(1));
    Logger::msg() << "Inseting new entry into " << m_name << " id = " << (int)record.id();
    auto pos = m_mapper[record.id()];
    auto file = bucket(pos.Bucket, true);
    auto it = (file->begin() + pos.Offset);
    it = record;
    file->recordAdded();
    for (auto& idx : m_indices)
    {
        idx.second->add(record);
    }
    return record.id();
}

void Collection::remove(uint64_t id)
{
    OperationDuration dur(Statistics::Type::Remove);
    Logger::msg() << "Removing entry id = " << (int)id;
    auto pos = m_mapper[id];
    auto file = bucket(pos.Bucket, false);
    if (file == nullptr)
        return;
    auto it = (file->begin() + pos.Offset);
    if (file->checkSignature(Record::RecordSignature, it.absolutePos()))
    {
        file->write(~RECORD_SIGNATURE, it.absolutePos());
        file->recordRemoved();
        if (file->header().rows() == 0)
        {
            removeBucket(pos.Bucket);
        }
    }
}

bool Collection::contains(uint64_t id)
{
    OperationDuration dur(Statistics::Type::GetById);
    auto pos = m_mapper[id];
    auto file = bucket(pos.Bucket, false);
    if (file == nullptr)
        return false;
    auto it = (file->begin() + pos.Offset);
    return file->checkSignature(Record::RecordSignature, it.absolutePos());
}

Record Collection::get(uint64_t id)
{
    OperationDuration dur(Statistics::Type::GetById);
    auto pos = m_mapper[id];

    auto file = bucket(pos.Bucket, false);

    if (file != nullptr)
    {
        auto it = (file->begin() + pos.Offset);
        if (file->checkSignature(Record::RecordSignature, it.absolutePos()))
            return *it;
    }

    return Record();
}
