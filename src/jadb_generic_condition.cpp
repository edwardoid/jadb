#include "jadb_generic_condition.h"
#include "jadb_condition_builder.h"

using namespace jadb;

GenericCondition::GenericCondition()
{

}

GenericCondition::~GenericCondition()
{

}

bool GenericCondition::create(const nlohmann::json& val)
{
    if (val.is_object() || val.is_array())
    {
        bool obj = val.is_object();
        for(auto i = val.begin(); i != val.end(); ++i)
        {
            Condition* c = nullptr;
            if(obj)
                c = ConditionBuilder::create(i.key(), *i);
            else
                c = ConditionBuilder::create("", *i);
                
            if (c == nullptr)
                return false;
            m_conditions.emplace_back(std::unique_ptr<Condition>(c));
        }
        return true;
    }
    return false;
}