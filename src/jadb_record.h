#ifndef JADB_RECORD_H
#define JADB_RECORD_H

#include "jadb_logger.h"
#include "jadb_serialization.h"
#include "jadb_iterative_file.h"
#include "jadb_signed.h"
#include "jadb_configuration.h"
#include <nlohmann/json.hpp>

#include <stdint.h>
#include <string>
#include <vector>
#include <atomic>
#include <sstream>


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
        Record(nlohmann::json& object);
        Record(const std::vector<uint8_t>& raw);
        ~Record();
        uint64_t id() const;
        void setId(uint64_t);
        void generateId();
        static const uint32_t RecordSignature;

        std::ostream& view(std::ostream& os);
    protected:
        const nlohmann::json& data() const { return m_data; }
        static std::atomic<uint64_t> NextId;
    protected:
        friend class RESTApi;
        friend class Serialization;
        friend class Index;
        nlohmann::json m_data;
    };

    template<>
    inline size_t SizeInfo<Record>::size()
    {
        return Configuration::maxRecordSize();
    }

    template<>
    inline void Serialization::serialize<Record>(const Record& obj)
    {
        SignedItem<RECORD_SIGNATURE>::sign(m_stream);
        Serialization::serialize(obj.m_data.dump());
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
        obj.m_data = std::move(nlohmann::json::parse(json));
    }
}

#endif // JADB_RECORD_H

