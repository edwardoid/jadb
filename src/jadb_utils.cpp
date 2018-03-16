#include "jadb_utils.h"

uint32_t jadb::djb2(const unsigned char *data, uint32_t len)
{
    uint32_t hash = 0;
    int c;

    while (len-- && (c = *data++))
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}