#ifndef JADB_HEADER_H
#define JADB_HEADER_H

#include <stdint.h>
#include <memory>

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

    private:
        bool m_dirty = false;
        uint32_t m_data_begin = 512 * 1024;
        uint32_t m_data_end = 0;
        uint32_t m_rows = 0;
    };
}

#endif // JADB_HEADER_H
