#include "jadb_record.h"
#include "jadb_logger.h"
#include "jadb_serialization.h"

using namespace jadb;
const uint32_t Record::RecordSignature = RECORD_SIGNATURE;


std::atomic<uint64_t> Record::NextId(1);

Record::Record(const Record& src)
{
    m_data = src.m_data;
}

Record::Record(Record&& src)
{
    m_data = std::move(src.m_data);
}

Record::Record(uint64_t id)
{
}

Record::Record(std::string json)
{
    m_data = std::move(nlohmann::json::parse(json));
    if (m_data.find("__id") == m_data.end())
    {
        m_data["__id"] = NextId.fetch_add(1);
    }
}

Record::Record(nlohmann::json& doc)
    : m_data(doc)
{
    if (m_data.find("__id") == m_data.end())
    {
        m_data["__id"] = NextId.fetch_add(1);
    }
}

Record::Record(const std::vector<uint8_t>& raw)
    : Record(std::string(raw.cbegin(), raw.cbegin() + raw.size()))
{}


std::ostream& Record::view(std::ostream& os)
{
    os << m_data;
    return os;
}
/*
std::string Record::operator[] (const char* prop) const
{
    return (*this)[std::string(prop)];
}

std::string Record::operator[] (const std::string& prop) const
{
    static std::string NoVal;
    return m_data.get<std::string>(prop, NoVal);
}
*/
uint64_t Record::id() const
{
    static const uint64_t NoId = 0;
    return m_data.value("__id", NoId);
}

void Record::setId(uint64_t id)
{
    if (m_data.find("__id") == m_data.end())
    {
        m_data["__id"] = id;
    }
    else
    {
        m_data["__id"] = NextId.fetch_add(1);
    }
}

void Record::generateId()
{
    setId(NextId.fetch_add(1));
}

Record::~Record()
{
}
