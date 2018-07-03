#include "jadb_query.h"
#include "jadb_condition_builder.h"

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

    if (!q.is_object())
    {
        return false;
    }


    for(nlohmann::json::const_iterator i = q.begin(); i != q.end(); ++i)
    {
        Condition* c = ConditionBuilder::create(i.key(), *i);
        if (c == nullptr)
        {
            return false;
        }
        m_q.emplace_back(std::unique_ptr<Condition>(c));
    }

    return m_q.size() > 0;
}

bool Query::exec(IterativeFile<Record>& file, std::set<uint64_t>* filter) const
{
    for (auto& c : m_q)
    {
        c->exec(file, filter);
    }

    return true;
}