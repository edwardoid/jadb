#ifndef JADB_QUERY_H
#define JADB_QUERY_H

#include "jadb_condition.h"
#include <vector>

namespace jadb
{
    class Query
    {
    public:
        Query();
        ~Query();
        bool create(const nlohmann::json& doc);
        bool exec(IterativeFile<Record>& file, std::set<uint64_t>* filter) const;
    private:
        std::vector<std::unique_ptr<Condition>> m_q;
    };
}

#endif // JADB_QUERY_H