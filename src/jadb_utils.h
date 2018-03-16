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
}


#endif // JADB_UTILS_H