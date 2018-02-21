#include "jadb_record.h"
#include "jadb_logger.h"
#include "jadb_serialization.h"

using namespace jadb;
const uint32_t Record::RecordSignature = 0xDEADBEEF;
const uint32_t Record::MaxRecordSize = 5000000; // 5MB


std::atomic<uint64_t> Record::NextId(1);

Record::Record(uint64_t id)
{
}

Record::Record(std::string json)
{
	std::istringstream is(json);
	try
	{
		boost::property_tree::read_json(is, m_data);
		setData(m_data);
	}
	catch (boost::property_tree::json_parser_error e)
	{
		Logger::err() << e.message();
	}
}

Record::Record(boost::property_tree::ptree props)
{
	setData(props);
}

Record::Record(const std::vector<uint8_t>& raw)
	: Record(std::string(raw.cbegin(), raw.cbegin() + raw.size()))
{}


std::ostream& Record::view(std::ostream& os)
{
	boost::property_tree::write_json(os, m_data);
	return os;
}

void Record::setData(boost::property_tree::ptree props)
{
	if (props.get<uint64_t>("__id", 0) == 0)
	{
		props.put<uint64_t>("__id", NextId.fetch_add(1));
	}

	m_data = props;
}

uint64_t Record::id()
{
	return m_data.get<uint64_t>("__id", 0);
}

void Record::setId(uint64_t id)
{
	m_data.put<uint64_t>("__id", id);
}

void Record::generateId()
{
	setId(NextId.fetch_add(1));
}

Record::~Record()
{
}
