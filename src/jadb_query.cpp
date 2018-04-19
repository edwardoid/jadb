#include "jadb_query.h"
#include "jadb_condition_builder.h"

using namespace jadb;

Query::Query()
{

}

Query::~Query()
{

}

bool Query::create(const rapidjson::Document& doc)
{
    auto it = doc.FindMember("q");
    if (!doc.HasMember("q"))
    {
        return false;
    }

    auto q = doc.FindMember("q");
    if (!q->value.IsObject())
    {
        return false;
    }

    auto i = q->value.MemberBegin();
    auto e = q->value.MemberEnd();

    for (; i != e; ++i)
    {
        std::string keyword(i->name.GetString());
        Condition* c = ConditionBuilder::create(keyword, i->value);
        if (c == nullptr)
        {
            return false;
        }
        m_q.emplace_back(std::unique_ptr<Condition>(c));
    }

    return false;
}

bool Query::exec(IterativeFile<Record>& file, std::set<uint64_t>* filter) const
{
    for (auto& c : m_q)
    {
        c->exec(file, filter);
    }

    return true;
}