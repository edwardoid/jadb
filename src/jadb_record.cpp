#include "jadb_record.h"
#include "jadb_logger.h"
#include "jadb_serialization.h"
#include <rapidjson/ostreamwrapper.h>

using namespace jadb;
const uint32_t Record::RecordSignature = RECORD_SIGNATURE;


std::atomic<uint64_t> Record::NextId(1);

Record::Record(const Record& src)
{
    m_data.CopyFrom(src.m_data, m_data.GetAllocator());
}

Record::Record(Record&& src)
{
    m_data.CopyFrom(src.m_data.Move(), m_data.GetAllocator());
}


Record::Record(uint64_t id)
{
}

Record::Record(std::string json)
{
    std::istringstream is(json);
    m_data.Parse(json.c_str());
    if (!m_data.HasMember("__id"))
    {
        m_data.AddMember("__id", NextId.fetch_add(1), m_data.GetAllocator());
    }
}

Record::Record(rapidjson::Document& doc)
{
    m_data.CopyFrom(doc, m_data.GetAllocator(), true);
    if (!m_data.HasMember("__id"))
    {
        m_data.AddMember("__id", NextId.fetch_add(1), m_data.GetAllocator());
    }
}

Record::Record(const std::vector<uint8_t>& raw)
    : Record(std::string(raw.cbegin(), raw.cbegin() + raw.size()))
{}


std::ostream& Record::view(std::ostream& os)
{
    rapidjson::OStreamWrapper osw(os);
    rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
    m_data.Accept(writer);
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
    if (m_data.HasMember("__id"))
    {
        return m_data.FindMember("__id")->value.Get<uint64_t>();
    }

    return 0;
}

void Record::setId(uint64_t id)
{
    if (m_data.HasMember("__id"))
    {
        m_data.FindMember("__id")->value.Set(id);
    }
    else
    {
        m_data.AddMember("__id", NextId.fetch_add(1), m_data.GetAllocator());
    }
}

void Record::generateId()
{
    setId(NextId.fetch_add(1));
}

Record::~Record()
{
}
