#ifndef JADB_QUERY_H
#define JADB_QUERY_H

#include <nlohmann/json.hpp>
#include <cpp-btree/btree_set.h>
#include <vector>

namespace jadb
{
    class Query
    {
    public:
        Query();
        ~Query();
        bool create(const nlohmann::json& doc);
        bool exec(const class Collection* collection, btree::btree_set<uint64_t>* filter) const;
    private:
        std::vector<std::unique_ptr<class Condition>> m_q;
    };
}

#endif // JADB_QUERY_H