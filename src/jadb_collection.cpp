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

DataFile& Collection::bucket(uint32_t bucket)
{
    auto path = m_path;
    path.append(std::to_string(bucket));
    auto fileIt = m_files.find(path.generic_string());
    if (fileIt == m_files.end())
    {
        m_files.insert(std::make_pair(path.generic_string(), std::make_shared<DataFile>(path, shared_from_this())));
    }

    return *(m_files.find(path.generic_string())->second.get());
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

uint64_t Collection::insert(Record& record)
{
    if (record.id() == 0)
        record.generateId();
    Logger::msg() << "Inseting new entry into " << m_name << " id = " << (int)record.id();
    auto pos = m_mapper[record.id()];
    auto& file = bucket(pos.Bucket);
    auto it = (file.begin() + pos.Offset);
    it = record;
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
