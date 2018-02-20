#include "jadb_collection.h"
#include "jadb_logger.h"
#include "jadb.h"

#define MAX_RECORD_LENGTH (512 * 1024)

using namespace jadb;

Collection::Collection(std::string name, boost::filesystem::path path, class Database* db)
	: m_name(name), m_path(path), m_db(db)
{
}


Collection::~Collection()
{
}

DataFile& Collection::bucket(uint32_t bucket)
{
	auto path = m_path;
	path.append(std::to_string(bucket));
	auto fileIt = m_files.find(path.generic_string());
	if (fileIt == m_files.end())
	{
		m_files.insert({ path.generic_string(), std::make_unique<DataFile>(path.generic_string()) });
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

void Collection::insert(Record& record)
{
	Logger::msg() << "Inseting new entry into " << m_name << " id = " << (int)record.id();
	auto pos = m_mapper[record.id()];
	auto& file = bucket(pos.Bucket);
	file.dataSeek(pos.Offset);
	auto recordData = record.raw();
	uint32_t len = recordData.size();
	file.write(reinterpret_cast<const uint8_t*>(&len), sizeof(len));
	file.dataSeek(pos.Offset + sizeof(len));
	file.write(recordData.data(), len);
	file.flush();
}

void Collection::remove(uint64_t id)
{

}

bool Collection::contains(uint64_t id)
{
	return false;
}

Record Collection::get(uint64_t id)
{
	auto pos = m_mapper[id];

	auto& file = bucket(pos.Bucket);
	file.dataSeek(pos.Offset);
	uint32_t len = 0;
	file.read(reinterpret_cast<uint8_t*>(&len), sizeof(len));
	std::vector<uint8_t> raw;
	raw.resize(len);
	file.dataSeek(pos.Offset + sizeof(len));
	file.read(raw.data(), len);

	return Record(raw);
}