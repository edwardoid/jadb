#ifndef JADB_GENERIC_CONDITIONS_H
#define JADB_GENERIC_CONDITIONS_H

#include "jadb_condition.h"
#include <vector>
#include <memory>

namespace jadb
{
    class MultiEntryCondition: public Condition
    {
    public:
        MultiEntryCondition() {}
        virtual ~MultiEntryCondition() {}
        virtual bool create(const nlohmann::json& doc) override;
    protected:
        std::vector<std::unique_ptr<Condition>> m_conditions;
    };

    class SingleEntryCondition: public Condition
    {
    public:
        SingleEntryCondition() {}
        virtual ~SingleEntryCondition() {}
        virtual bool create(const nlohmann::json& doc) override;
    protected:
        nlohmann::json m_props;
    };
}


#endif // JADB_GENERIC_CONDITIONS_H