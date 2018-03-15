#ifndef JADB_SIGNED_H
#define JADB_SIGNED_H

#include <memory>
#include "jadb_file.h"

namespace jadb
{
    template<uint32_t Val>
    struct SignedItem
    {
        SignedItem() {}
        static const uint32_t ValidSignature = Val;
        static const uint32_t InvalidSignature = ~Val;
        static void sign(std::shared_ptr<File> file)
        {
            file->write(reinterpret_cast<const char*>(&ValidSignature), sizeof(ValidSignature));
        }

        static void invalidate(std::shared_ptr<File> file)
        {
            file->write(reinterpret_cast<const char*>(&InvalidSignature), sizeof(InvalidSignature));
        }

        static bool isSigned(std::shared_ptr<File> file)
        {
            uint32_t sig = InvalidSignature;
            file->read(reinterpret_cast<char*>(&sig), sizeof(ValidSignature));
            return sig == ValidSignature;
        }
    };
}

#endif // JADB_SIGNED_H