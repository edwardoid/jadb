#ifndef JADB_INDEX_FILE_H
#define JADB_INDEX_FILE_H

#include <memory>
#include <string>
#include <vector>

#include "jadb_file.h"

namespace jadb
{
    class Collection;

    class IndexFile
    {
    public:
        IndexFile(std::shared_ptr<Collection> collection);
        std::vector<std::string> fields() const;
        std::string name() const;
        uint32_t size() const;
    private:

    };
}

#endif // JADB_INDEX_FILE_H