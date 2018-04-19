#include "jadb_generic_condition.h"
#include "jadb_condition_builder.h"

using namespace jadb;

GenericCondition::GenericCondition()
{

}

GenericCondition::~GenericCondition()
{

}

bool GenericCondition::create(const rapidjson::Value& val)
{
    if (val.IsObject())
    {
        for (auto i = val.MemberBegin(); i != val.MemberEnd(); ++i)
        {
            std::string kw(i->name.GetString());
            Condition* c = ConditionBuilder::create(kw, i->value);
            if (c == nullptr)
                return false;
            m_conditions.emplace_back(std::unique_ptr<Condition>(c));
        }
        return true;
    }
    else if (val.IsArray())
    {
        auto array = val.GetArray();
        for (auto& obj : array)
        {
            Condition* c = ConditionBuilder::create("", obj);
            if (c == nullptr)
                return false;
            m_conditions.emplace_back(std::unique_ptr<Condition>(c));
        }
        return true;
    }
    return false;
}