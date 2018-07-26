#ifndef JADB_CONDITION_BUILDER_H
#define JADB_CONDITION_BUILDER_H

#include <nlohmann/json.hpp>
#include "jadb_condition.h"
#include <string>
#include <unordered_map>

namespace jadb
{
    class ConditionBuilder
    {
    public:
        enum class Type
        {
            Unsupported = 0,
            Equal,
            NotEqual,
            Greater,
            GreaterOrEqual,
            Less,
            LessOrEqual,
            And,
            Or,
            Xor,
            Not
        };

        static Type type(const std::string& key);
        static Type type(const nlohmann::json& obj, bool& hasKey);
        static Condition* create(const nlohmann::json& obj);
    private:
        static const std::unordered_map<std::string, Type> m_typesMapping;
    };
}


#endif // JADB_CONDITION_BUILDER_H
