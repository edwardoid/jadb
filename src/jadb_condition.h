#ifndef JADB_CONDITION_H
#define JADB_CONDITION_H

#include <rapidjson/document.h>
#include <memory>

namespace jadb
{
    class Condition
    {
    public:
        virtual bool create(const rapidjson::Value& obj) = 0;
        virtual bool exec() const = 0;
        virtual ~Condition() {};
    };
}

#endif // JADB_CONDITION_H