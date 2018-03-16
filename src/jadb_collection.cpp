#include "jadb_collection.h"
#include "jadb_logger.h"
#include "jadb.h"
#include <memory>

#define MAX_RECORD_LENGTH (512 * 1024)

using namespace jadb;

Collection::Collection(std::string name, boost::filesystem::path path, class Database* db)
    : m_name(name), m_path(path), m_db(db)
{
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
    std::vector<Record> res;
    auto idx = m_indices.find(indexFilePath(index).generic_string());
    if (idx == m_indices.end())
        return res;

    boost::property_tree::ptree query;
    for (auto q : filter)
    {
        query.put<std::string>(q.first, q.second);
    }
    auto ids = idx->second->get(query, skip, limit);
    for (auto id : ids)
    {
        res.emplace_back(get(id));
    }
    return res;
}

DataFile& Collection::bucket(uint32_t bucket)
{
    auto path = m_path;
    path.append(std::to_string(bucket));
    auto fileIt = m_data.find(path.generic_string());
    if (fileIt == m_data.end())
    {
        m_data.insert(std::make_pair(path.generic_string(), std::make_shared<DataFile>(path, shared_from_this())));
    }

    return *(m_data.find(path.generic_string())->second.get());
}

boost::filesystem::path Collection::indexFilePath(const std::string& name) const
{
    auto path = m_path;
    path.append("idx");
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
    if (record.id() == 0)
        record.generateId();
    Logger::msg() << "Inseting new entry into " << m_name << " id = " << (int)record.id();
    auto pos = m_mapper[record.id()];
    auto& file = bucket(pos.Bucket);
    auto it = (file.begin() + pos.Offset);
    it = record;
    for (auto& idx : m_indices)
    {
        idx.second->add(record);
    }
    return record.id();
}

void Collection::remove(uint64_t id)
{
    Logger::msg() << "Removing entry id = " << (int)id;
    auto pos = m_mapper[id];
    auto& file = bucket(pos.Bucket);
    auto it = (file.begin() + pos.Offset);
    file.write(~RECORD_SIGNATURE, it.absolutePos());
}

bool Collection::contains(uint64_t id)
{
    auto pos = m_mapper[id];
    auto& file = bucket(pos.Bucket);
    auto it = (file.begin() + pos.Offset);
    return file.checkSignature(Record::RecordSignature, it.absolutePos());
}

Record Collection::get(uint64_t id)
{
    auto pos = m_mapper[id];

    auto& file = bucket(pos.Bucket);

    auto it = (file.begin() + pos.Offset);
    if (file.checkSignature(Record::RecordSignature, it.absolutePos()))
        return *it;

    return Record();
}
