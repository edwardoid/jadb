#include "jadb_logic_conditions.h"

using namespace jadb;

And::And() {}
And::~And() {}

bool And::exec() const
{
    for (auto& c : m_conditions)
    {
        if (c->exec())
        {
            return false;
        }
    }

    return true;
}

Or::Or() {}
Or::~Or() {}

bool Or::exec() const
{
    for (auto& c : m_conditions)
    {
        if (c->exec())
        {
            return true;
        }
    }

    return false;
}