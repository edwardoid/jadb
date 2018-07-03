#include "jadb_condition_builder.h"
#include "jadb_logic_conditions.h"
#include "jadb_equal_condition.h"

/*
    Condition queries:

    {
        "a" == "a",
        "$not" : [
            {
                "$eq" : {
                    "name" : "edward",
                    "service" : "github"
                }
            },
            {
                "repo" : "jadb"
            }
        ]
    }
*/

using namespace jadb;

const std::unordered_map<std::string, ConditionBuilder::Type> ConditionBuilder::m_typesMapping(
    {
        { "$eq" ,   ConditionBuilder::Type::Equal },
        { "$neq" ,  ConditionBuilder::Type::NotEqual},
        { "$gt" ,   ConditionBuilder::Type::Greater },
        { "$gte" ,  ConditionBuilder::Type::GreaterOrEqual },
        { "$lt" ,   ConditionBuilder::Type::Less },
        { "$lte" ,  ConditionBuilder::Type::LessOrEqual },
        { "$and" ,  ConditionBuilder::Type::And },
        { "$or" ,   ConditionBuilder::Type::Or },
        { "$xor" ,   ConditionBuilder::Type::Xor },
        { "$not" ,   ConditionBuilder::Type::Not }
    }
);

ConditionBuilder::Type ConditionBuilder::type(const std::string& keyword)
{
    auto it = m_typesMapping.find(keyword);
    if (it == m_typesMapping.cend())
        return ConditionBuilder::Type::Unsupported;
    return it->second;
}

ConditionBuilder::Type ConditionBuilder::type(const nlohmann::json& obj, bool& hasKey)
{
    Type tp = Type::Unsupported;
    if (obj.size() == 1)
    {
        tp = type(*obj.cbegin());
        hasKey = tp != Type::Unsupported;
    }
    if (tp == Type::Unsupported)
        tp = Type::Equal;
    return tp;
}

Condition* ConditionBuilder::create(const std::string& key, const nlohmann::json& val)
{
    Condition* c = nullptr;

    Type tp = Type::Unsupported;

    nlohmann::json data;
    if (key.empty())
    {
        bool hasKey = false;
        tp = type(val, hasKey);
        if (tp != Type::Unsupported && hasKey)
        {
            data = *(val.begin());
        }
    }
    else
    {
        tp = type(key);
        data = val;
    }

    switch (tp)
    {
    case Type::Or: {
        c = new Or();
        break;
    }
    case Type::And: {
        c = new And();
        break;
    }
    case Type::Equal: {
        c = new Equal();
        break;
    }
    case Type::NotEqual: {
        c = new NotEqual();
        break;
    }
    case Type::Less: {
        c = new Less();
        break;
    }
    case Type::LessOrEqual: {
        c = new LessOrEqual();
        break;
    }
    case Type::Greater: {
        c = new Greater();
        break;
    }
    case Type::GreaterOrEqual: {
        c = new GreaterOrEqual();
        break;
    }
    default:
        break;
    }
    if (c != nullptr)
    {
        if (!c->create(data))
        {
            delete c;
            c = nullptr;
        }
    }
    return c;
}