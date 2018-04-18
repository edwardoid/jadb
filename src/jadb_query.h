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
        bool create(const rapidjson::Document& doc);
        bool exec() const;
    private:
        std::vector<std::unique_ptr<Condition>> m_q;
    };
}

#endif // JADB_QUERY_H