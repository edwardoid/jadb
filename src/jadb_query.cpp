#include "jadb_query.h"
#include "jadb_collection.h"
#include "jadb_condition_builder.h"
#include <cpp-btree/btree_set.h>

using namespace jadb;

Query::Query()
{

}

Query::~Query()
{

}

bool Query::create(const nlohmann::json& doc)
{
    auto it = doc.find("q");
    if (it == doc.end())
    {
        return false;
    }

    auto& q = *it;

    if (!q.is_object() || q.size() != 1)
    {
        return false;
    }

    m_q = std::unique_ptr<Condition>(ConditionBuilder::create(q));
    return m_q != nullptr;
}

bool Query::exec(const Collection* collection, btree::btree_set<uint64_t>*& filter) const
{
    return m_q->exec(*collection, filter);
}