#ifndef JADB_GENERIC_CONDITIONS_H
#define JADB_GENERIC_CONDITIONS_H

#include "jadb_condition.h"
#include <vector>
#include <memory>

namespace jadb
{
    class GenericCondition : public Condition
    {
    public:
        GenericCondition();
        virtual ~GenericCondition();
        virtual bool create(const rapidjson::Value& doc) override;
    protected:
        std::vector<std::unique_ptr<Condition>> m_conditions;
    };
}


#endif // JADB_GENERIC_CONDITIONS_H