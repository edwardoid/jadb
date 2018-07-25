#ifndef JADB_LOGIC_CONDITIONS_H
#define JADB_LOGIC_CONDITIONS_H

#include "jadb_generic_condition.h"

namespace jadb
{
    class And : public GenericCondition
    {
    public:
        And();
        virtual ~And();
        virtual bool exec(IterativeFile<Record>& file, std::set<uint64_t>* filter) const override;
    };

    class Or : public GenericCondition
    {
    public:
        Or();
        virtual ~Or();
        virtual bool exec(IterativeFile<Record>& file, std::set<uint64_t>* filter) const override;
    };
}

#endif // JADB_LOGIC_CONDITIONS_H