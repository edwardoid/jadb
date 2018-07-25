#include "jadb_logic_conditions.h"

using namespace jadb;

And::And() {}
And::~And() {}

bool And::exec(const Collection& collection, btree::btree_set<uint64_t>*& filter) const
{
    for (auto& c : m_conditions)
    {
        if (c->exec(collection, filter))
        {
            return false;
        }
    }

    return true;
}

Or::Or() {}
Or::~Or() {}

bool Or::exec(const Collection& collection, btree::btree_set<uint64_t>*& filter) const
{
    for (auto& c : m_conditions)
    {
        if (c->exec(collection, filter))
        {
            return true;
        }
    }

    return false;
}