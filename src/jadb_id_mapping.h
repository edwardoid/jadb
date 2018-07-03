#ifndef JADB_ID_MAPPING_H
#define JADB_ID_MAPPING_H

#include <unordered_map>
#include "jadb_serialization.h"

namespace jadb
{
    template<typename IdType = uint64_t, typename OffsetType = uint64_t, class Container = std::unordered_map<IdType, OffsetType>>
    class IdMapping
    {
    public:
        IdMapping() {}
        ~IdMapping() {}
        bool has(uint64_t id) const { return m_map.find(id) != m_map.end(); }
        uint64_t offset(uint64_t id) const { return m_map.find(id)->second; }
        void insert(uint64_t id) { m_map.emplace({  id, id }); }
        typename Container::const_iterator begin() const { return m_map.cbegin(); }
        typename Container::const_iterator end() const { return m_map.cend(); }
    private:
        Container m_map;
    };

    using STLHashMapMapping = IdMapping<uint64_t, uint64_t, std::unordered_map<uint64_t, uint64_t> >;    

    template<>
    inline void Serialization::serialize<IdMapping>(const IdMapping& obj)
    {
        Serialization::serialize(obj.m_map));
    }

    template<>
    inline void Serialization::deserialize(Record& obj)
    {
        Serialization::serialize(obj.m_map));
        std::string json;
        if(!SignedItem<RECORD_SIGNATURE>::isSigned(m_stream))
        {
            throw std::runtime_error("Bad signature for record");
        }
        Serialization::deserialize(obj.m_map);
        obj.m_data = std::move(nlohmann::json::parse(json));
    }
}

#endif // JADB_ID_MAPPING_H