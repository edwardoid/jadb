#include "jadb_collection.h"
#include "jadb_logger.h"
#include "jadb_database.h"
#include "jadb_filesystem.h"
#include "jadb_syncronizer.h"
#include "jadb_stats.h"
#include "jadb_utils.h"
#include <memory>
#include <vector>
#include <cpp-btree/btree_set.h>

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


    bool newCollection = !boost::filesystem::exists(m_path);
    auto file = FileSystem::Get(m_path);
    file->close();
    file->open(std::ios::in | std::ios::binary);
    file->lock();
    uint64_t nextId = 1;

    if(!newCollection)
    {
        Serialization(file).deserialize(nextId);
        ArraySerialization(file).deserialize<std::string>(indices);
        ArraySerialization(file).deserialize<std::string>(data);
        Serialization(file).deserialize(m_records);
        
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
    }
    m_ids.store(nextId);
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

        Serialization(file).serialize(static_cast<uint64_t>(m_ids.load()));
        ArraySerialization(file).serialize<std::string>(indices);
        ArraySerialization(file).serialize<std::string>(data);
        Serialization(file).serialize(m_records);
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

    auto b = recordsBegin();
    auto e = recordsEnd();

    for(; b != e; ++b)
    {
        index->add(get(b->first, true));
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

std::vector<Record> Collection::query(const Query& query, size_t& all, size_t limit, size_t skip)
{
    std::vector<Record> res;
    btree::btree_set<uint64_t>* filter = nullptr;
    for (auto& file : m_data)
    {
        if (!query.exec(this, filter))
        {
            return std::vector<Record>();
        }
    }

    all = 0;
    if(filter != nullptr)
    {
        all = filter->size();
        auto b = filter->begin();
        while(skip-- && b != filter->end())
            ++b;
        auto e = b;
        while(limit-- && e != filter->end())
            ++e;

        while(b != e)
        {
            res.emplace_back(get(*b, true));
            ++b;
        }
    }
    return res;
}

IdMapping::ConstIterator Collection::recordsBegin() const
{
    return m_records.begin();
}

IdMapping::ConstIterator Collection::recordsEnd() const
{
    return m_records.end();
}

std::shared_ptr<DataFile> Collection::bucket(uint32_t bucket, bool create) const
{
    auto path = m_dataDir;
    path.append(std::to_string(bucket));
    auto fileIt = m_data.find(path.generic_string());
    if (fileIt == m_data.end())
    {
        if (create)
        {
            m_data.insert(std::make_pair(path.generic_string(), std::make_shared<DataFile>(path, const_cast<Collection*>(this))));
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
    auto hashValue = hash(record.id());
    auto pos = m_mapper[hashValue];
    auto file = bucket(pos.Bucket, true);
    auto it = (file->begin() + pos.Offset);
    it = record;
    m_records.insert(record.id(), hashValue);
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
    m_records.remove(id);
    auto pos = m_mapper[hash(id)];
    auto file = bucket(pos.Bucket, false);
    if (file == nullptr)
        return;
    auto it = (file->begin() + pos.Offset);
    if (file->checkSignature(RECORD_SIGNATURE, it.absolutePos()))
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
    return m_records.has(id);
}

Record Collection::get(uint64_t id, bool fast) const
{
    OperationDuration dur(Statistics::Type::GetById);

    if(m_records.has(id))
    {
        auto pos = m_mapper[m_records.key(id)];

        auto file = bucket(pos.Bucket, false);

        if (file != nullptr)
        {
            auto it = (file->begin() + pos.Offset);
            if (file->checkSignature(RECORD_SIGNATURE, it.absolutePos()))
                return *it;
        }
    }

    return Record();
}
