#ifndef JADB_RECORD_H
#define JADB_RECORD_H

#include <stdint.h>
#include <string>
#include <vector>
#include <boost\property_tree\ptree.hpp>
#include <atomic>

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
	private:
		void setData(boost::property_tree::ptree object);
		static std::atomic<uint64_t> NextId;
	private:
		boost::property_tree::ptree m_data;
	};
}

#endif // JADB_RECORD_H

