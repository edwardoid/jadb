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
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>


#define RECORD_SIGNATURE 0xDEADBEEF

namespace jadb
{
    class Record
    {
    public:
        Record(const Record& src);
        Record(Record&& src);
        Record(uint64_t id = 0);
        Record(std::string json);
        Record(rapidjson::Document& object);
        Record(const std::vector<uint8_t>& raw);
        ~Record();
        uint64_t id() const;
        void setId(uint64_t);
        void generateId();
        static const uint32_t RecordSignature;

        std::ostream& view(std::ostream& os);
        /*
        std::string operator[] (const char* prop) const;
        std::string operator[] (const std::string& prop) const;
        */

    protected:
        const rapidjson::Document& data() const { return m_data; }
        static std::atomic<uint64_t> NextId;
    protected:
        friend class RESTApi;
        friend class Serialization;
        friend class Index;
        rapidjson::Document m_data;
    };

    template<>
    inline size_t SizeInfo<Record>::size()
    {
        return Configuration::maxRecordSize();
    }

    template<>
    inline void Serialization::serialize<Record>(const Record& obj)
    {
        rapidjson::StringBuffer buffer;

        buffer.Clear();

        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        obj.m_data.Accept(writer);

        SignedItem<RECORD_SIGNATURE>::sign(m_stream);
        Serialization::serialize(std::string(buffer.GetString()));
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
        obj.m_data.Parse(json.c_str());
    }
}

#endif // JADB_RECORD_H

