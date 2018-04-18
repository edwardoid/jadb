#include "jadb_condition_builder.h"
#include "jadb_logic_conditions.h"
#include "jadb_equal_condition.h"

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

ConditionBuilder::Type ConditionBuilder::type(const rapidjson::Value::ConstObject& obj, bool& hasKey)
{
    auto count = obj.MemberCount();
    if (count == 0)
        return Type::Unsupported;

    Type tp = Type::Unsupported;

    if (count == 1)
    {
        auto first = obj.MemberBegin();
        tp = type(std::string(first->name.GetString()));
        hasKey = tp != Type::Unsupported;
    }
    if (tp == Type::Unsupported)
        tp = Type::Equal;
    return tp;
}

ConditionBuilder::Type ConditionBuilder::type(const rapidjson::Value::Object& obj, bool& hasKey)
{
    auto count = obj.MemberCount();
    if (count == 0)
        return Type::Unsupported;
    
    Type tp = Type::Unsupported;

    if (count == 1)
    {
        auto first = obj.MemberBegin();
        if (first->value.IsArray())
            tp = type(std::string(first->name.GetString()));
        hasKey = type(first->name.GetString()) != Type::Unsupported;
    }
    if (tp == Type::Unsupported)
        tp = Type::Equal;
    return tp;
}

Condition* ConditionBuilder::create(const std::string& key, const rapidjson::Value& val)
{
    Condition* c = nullptr;

    Type tp = Type::Unsupported;

    const rapidjson::Value* data = &val;
    if (key.empty())
    {
        rapidjson::Value::Object obj = const_cast<rapidjson::Value&>(val).GetObject();
        bool hasKey = false;
        tp = type(obj, hasKey);
        if (tp != Type::Unsupported && hasKey)
        {
            data = &(val.MemberBegin()->value);
        }
    }
    else
    {
        tp = type(key);
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
        if (!c->create(*data))
        {
            delete c;
            c = nullptr;
        }
    }
    return c;
}