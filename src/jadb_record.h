#ifndef JADB_RECORD_H
#define JADB_RECORD_H

#include "jadb_logger.h"
#include "jadb_serialization.h"
#include "jadb_iterative_file.h"
#include "jadb_signed.h"
#include "jadb_configuration.h"

#include <stdint.h>
#include <string>
#include <vector>
#include <atomic>
#include <sstream>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>


#define RECORD_SIGNATURE 0xDEADBEEF

namespace jadb
{
    class Record
    {
    public:
        Record(uint64_t id = 0);
        Record(std::string json);
        Record(boost::property_tree::ptree object);
        Record(const std::vector<uint8_t>& raw);
        ~Record();
        uint64_t id() const;
        void setId(uint64_t);
        void generateId();
        static const uint32_t RecordSignature;

        std::ostream& view(std::ostream& os);

        std::string operator[] (const char* prop) const;
        std::string operator[] (const std::string& prop) const;

    protected:
        void setData(boost::property_tree::ptree object);
        const boost::property_tree::ptree& data() const { return m_data; }
        static std::atomic<uint64_t> NextId;
    protected:
        friend class RESTApi;
        friend class Serialization;
        boost::property_tree::ptree m_data;
    };

    template<>
    inline size_t SizeInfo<Record>::size()
    {
        return Configuration::maxRecordSize();
    }

    template<>
    inline void Serialization::serialize<Record>(const Record& obj)
    {
        std::ostringstream ss;
        boost::property_tree::write_json(ss, obj.data(), false);
        std::string str = ss.str();
        SignedItem<RECORD_SIGNATURE>::sign(m_stream);
        Serialization::serialize(str);
    }

    template<>
    inline void Serialization::deserialize(Record& obj)
    {
        std::string json;
        if(!SignedItem<RECORD_SIGNATURE>::isSigned(m_stream))
        {
            throw std::runtime_error("Bad signature for record");
        }
        Serialization::deserialize(json);
        std::istringstream is(json);
        boost::property_tree::read_json(is, obj.m_data);
    }
}

#endif // JADB_RECORD_H

