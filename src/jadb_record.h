#ifndef JADB_RECORD_H
#define JADB_RECORD_H

#include "jadb_logger.h"

#include <stdint.h>
#include <string>
#include <vector>
#include <atomic>
#include <sstream>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>


namespace jadb
{
	class Record
	{
	public:
		Record(uint64_t id);
		Record(std::string json);
		Record(boost::property_tree::ptree object);
		Record(const std::vector<uint8_t>& raw);
		~Record();
		uint64_t id();
		void setId(uint64_t);
		std::vector<uint8_t> raw() const;
	
		template<class Archive>
		void save(Archive& ar, const unsigned int version) const
		{
			std::ostringstream ss;
			boost::property_tree::write_json(ss, m_data, false);
			std::string str = ss.str();
			ar & str;
		}

		template<class Archive>
		void load(Archive& ar, const unsigned int version)
		{
			std::string json;
			ar & json;
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

		BOOST_SERIALIZATION_SPLIT_MEMBER()
	private:
		void setData(boost::property_tree::ptree object);
		static std::atomic<uint64_t> NextId;
	private:
		boost::property_tree::ptree m_data;
	};
}

#endif // JADB_RECORD_H

