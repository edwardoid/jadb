#ifndef JADB_UTILS_H
#define JADB_UTILS_H

#include <inttypes.h>

namespace jadb
{
    uint32_t djb2(const unsigned char *data, uint32_t len);

    inline uint32_t hash(const unsigned char *data, uint32_t len)
    {
        return djb2(data, len);
    }

    inline uint64_t hash(uint64_t x)
    {
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        x = x ^ (x >> 31);
        return x;
    }
}


#endif // JADB_UTILS_H