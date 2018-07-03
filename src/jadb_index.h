#ifndef JADB_INDEX_H
#define JADB_INDEX_H

#include <vector>
#include <string>
#include "jadb_record.h"
#include "jadb_serialization.h"

namespace jadb
{
    class Index
    {
    public:
        static const uint32_t CantCreateIndex;
        Index(std::vector<std::string> fields = { "_id" });
        uint32_t operator()(const Record& rec) const;
        uint32_t operator()(const nlohmann::json& tree) const;
        const std::vector<std::string>& fields() const;
    protected:
        friend class Serialization;
        std::vector<std::string> m_fields;
    };

    template<>
    inline void Serialization::serialize<Index>(const Index& obj)
    {
        auto& fields = obj.fields();
        uint32_t len = static_cast<uint32_t>(fields.size());
        Serialization::serialize(len);
        for (auto& f : fields)
            Serialization::serialize(f);
    }

    template<>
    inline void Serialization::deserialize(Index& obj)
    {
        obj.m_fields.clear();
        uint32_t len = 0;
        Serialization::deserialize(len);
        obj.m_fields.reserve(len);
        while (len--)
        {
            std::string f;
            Serialization::deserialize(f);
            if (f.empty())
                continue;
            obj.m_fields.push_back(f);
        }
    }
}

#endif // JADB_INDEX_H
