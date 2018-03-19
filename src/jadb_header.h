#ifndef JADB_HEADER_H
#define JADB_HEADER_H

#include <stdint.h>
#include <memory>
#include "jadb_serialization.h"
#include "jadb_iterative_file.h"

namespace jadb
{

    class Header
    {
    public:
        Header();
        ~Header();
        
        void setRows(uint32_t rows);
        uint32_t begin() const;
        uint32_t end() const;
        uint32_t rows() const;

    protected:
        friend class Serialization;
        bool m_dirty = false;
        uint32_t m_data_begin = sizeof(uint32_t) * 3;
        uint32_t m_data_end = 0;
        uint32_t m_rows = 0;
    };

    template<>
    inline void Serialization::serialize<Header>(const Header& obj)
    {
        serialize(obj.m_data_begin);
        serialize(obj.m_data_end);
        serialize(obj.m_rows);
    }

    template<>
    inline void Serialization::deserialize(Header& obj)
    {
        deserialize(obj.m_data_begin);
        deserialize(obj.m_data_end);
        deserialize(obj.m_rows);
    }

    template<>
    inline size_t SizeInfo<Header>::size()
    {
        return sizeof(uint32_t) * 3;
    }
}

#endif // JADB_HEADER_H
