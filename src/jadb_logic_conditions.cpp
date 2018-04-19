#include "jadb_logic_conditions.h"

using namespace jadb;

And::And() {}
And::~And() {}

bool And::exec(IterativeFile<Record>& file, std::set<uint64_t>* filter) const
{
    for (auto& c : m_conditions)
    {
        if (c->exec(file, filter))
        {
            return false;
        }
    }

    return true;
}

Or::Or() {}
Or::~Or() {}

bool Or::exec(IterativeFile<Record>& file, std::set<uint64_t>* filter) const
{
    for (auto& c : m_conditions)
    {
        if (c->exec(file, filter))
        {
            return true;
        }
    }

    return false;
}