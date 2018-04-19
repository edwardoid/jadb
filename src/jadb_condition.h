#ifndef JADB_CONDITION_H
#define JADB_CONDITION_H

#include <rapidjson/document.h>
#include <memory>
#include <set>
#include "jadb_iterative_file.h"
#include "jadb_record.h"

namespace jadb
{
    class Condition
    {
    public:
        virtual bool create(const rapidjson::Value& obj) = 0;
        virtual bool exec(IterativeFile<Record>& file, std::set<uint64_t>* filter) const = 0;
        virtual ~Condition() {};
    };
}

#endif // JADB_CONDITION_H