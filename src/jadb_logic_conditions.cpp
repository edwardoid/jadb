#include "jadb_logic_conditions.h"

using namespace jadb;

And::And() {}
And::~And() {}

bool And::exec(const Collection& collection, btree::btree_set<uint64_t>*& filter) const
{
    for (auto& c : m_conditions)
    {
        if (!c->exec(collection, filter))
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
    bool res = false;
    for (auto& c : m_conditions)
    {
        btree::btree_set<uint64_t>* f = nullptr;

        if (c->exec(collection, f))
        {
            res = true;
            if(f != nullptr)
            {
                if(filter == nullptr)
                    filter = new btree::btree_set<uint64_t>();
                filter->insert(f->begin(), f->end());
                delete f;
            }
        }
    }

    return res;
}