#include "jadb_generic_conditions.h"
#include "jadb_condition_builder.h"

using namespace jadb;

bool MultiEntryCondition::create(const nlohmann::json& val)
{
    if(!val.is_array())
        return false;

    for(auto i : val)
    {
        auto c = ConditionBuilder::create(i);
        if(c == nullptr)
            return false;
        m_conditions.emplace_back(std::unique_ptr<Condition>(c));
    }
    return m_conditions.size() != 0;
}

bool SingleEntryCondition::create(const nlohmann::json& val)
{
    if(!val.is_object())
        return false;
    for(auto i : val)
    {
        if(i.is_object() || i.is_array())
            return false;
    }
    m_props = val;
    return true;
}