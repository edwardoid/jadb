#include "jadb_record.h"
#include "jadb_logger.h"

using namespace jadb;

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

void Record::setData(boost::property_tree::ptree props)
{
	if (props.get<uint64_t>("__id", 0) == 0)
	{
		props.put("__id", NextId.fetch_add(1));
	}

	m_data = props;
}

uint64_t Record::id()
{
	return m_data.get<uint64_t>("__id", 0);
}

void Record::setId(uint64_t id)
{
	m_data.put("__id", id);
}

std::vector<uint8_t> Record::raw() const
{
	std::ostringstream ss;
	boost::property_tree::write_json(ss, m_data, false);
	auto str = ss.str();
	return std::vector<uint8_t>(str.cbegin(), str.cend());
}

Record::~Record()
{
}
