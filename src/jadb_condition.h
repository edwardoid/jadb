#ifndef JADB_CONDITION_H
#define JADB_CONDITION_H

#include "jadb_collection.h"
#include <nlohmann/json.hpp>
#include <cpp-btree/btree_set.h>

namespace jadb
{
    class Condition
    {
    public:
        virtual bool create(const nlohmann::json& obj) = 0;
        virtual bool exec(const Collection& collection, btree::btree_set<uint64_t>*& filter) const = 0;
        virtual ~Condition() {};
    };
}

#endif // JADB_CONDITION_H
