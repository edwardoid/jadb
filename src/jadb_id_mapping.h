#ifndef JADB_ID_MAPPING_H
#define JADB_ID_MAPPING_H

#include <unordered_map>
#include "jadb_serialization.h"

namespace jadb
{
    class IdMapping
    {
    public:
        using IdType = uint64_t;
        using MappedType = uint64_t;
        using Container = std::unordered_map<IdType, MappedType>;
        using ConstIterator = Container::const_iterator;
        IdMapping() {}
        ~IdMapping() {}
        bool has(IdType id) const { return m_map.find(id) != m_map.end(); }
        uint64_t key(IdType id) const { return m_map.find(id)->second; }
        void insert(IdType id, MappedType map) { m_map.insert({  id, map }); }
        void insert(std::pair<IdType, MappedType> item) { m_map.insert(item); }
        void remove(IdType id) { m_map.erase(id); }
        size_t size() const { return m_map.size(); }
        ConstIterator begin() const { return m_map.cbegin(); }
        ConstIterator end() const { return m_map.cend(); }
    protected:
        friend class Serialization;
        std::unordered_map<IdType, MappedType> m_map;
    };

    template<>
    inline void Serialization::serialize<IdMapping>(const IdMapping& obj)
    {
        MapSerialization(m_stream).serialize<IdMapping::Container::key_type, IdMapping::Container::mapped_type, IdMapping::Container, NumberSerialization, NumberSerialization>(obj.m_map);
    }

    template<>
    inline void Serialization::deserialize(IdMapping& obj)
    {
        MapSerialization(m_stream).deserialize<IdMapping::Container::key_type, IdMapping::Container::mapped_type, IdMapping::Container, NumberSerialization, NumberSerialization>(obj.m_map);
    }

}

#endif // JADB_ID_MAPPING_H