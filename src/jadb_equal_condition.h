#ifndef JADB_EQUAL_CONDITION_H
#define JADB_EQUAL_CONDITION_H

#include "jadb_condition.h"
#include <unordered_map>
#include <algorithm>
#include <functional>

namespace jadb
{
    template<bool Result, typename Predicate>
    class EqualCondition: public Condition
    {
    public:
        EqualCondition() {}
        virtual ~EqualCondition() {}
        virtual bool create(const rapidjson::Value& obj) override
        {
            if (!obj.IsObject())
            {
                return false;
            }
            for (auto i = obj.MemberBegin(); i != obj.MemberEnd(); ++i)
            {
                std::string key = i->name.GetString();
                if (key.empty())
                {
                    return false;
                }

                const auto& src = i->value;

                if (src.IsObject())
                {
                    bool hasKey = false;
                    auto obj = src.GetObject();
                    ConditionBuilder::Type type = ConditionBuilder::type(obj, hasKey);
                    if (!hasKey)
                        return false;
                    switch (type)
                    {
                        case ConditionBuilder::Type::Less:
                        {
                            createFilter<std::less<rapidjson::Value>>(key, obj.MemberBegin()->value);
                            break;
                        }
                        case ConditionBuilder::Type::LessOrEqual:
                        {
                            createFilter<std::less_equal<rapidjson::Value>>(key, obj.MemberBegin()->value);
                            break;
                        }
                        case ConditionBuilder::Type::Greater:
                        {
                            createFilter<std::greater<rapidjson::Value>>(key, obj.MemberBegin()->value);
                            break;
                        }
                        case ConditionBuilder::Type::GreaterOrEqual:
                        {
                            createFilter<std::greater_equal<rapidjson::Value>>(key, obj.MemberBegin()->value);
                            break;
                        }
                        default:
                        {
                            return false;
                        }
                    }
                }
                else
                {
                    createFilter<Predicate>(key, src);
                }
            }

            return true;
        }
        virtual bool exec(IterativeFile<Record>& file, std::set<uint64_t>* filter) const override
        {
            return true;
        }
    private:

        template<typename BinaryPredicate>
        void createFilter(const std::string& key, const rapidjson::Value& src)
        {
            Cmp c;
            c.key = key;
            static rapidjson::Value::AllocatorType alloc;
            c.to.CopyFrom(src, alloc, true);

            c.fn = BinaryPredicate();
            m_filter.emplace_back(c);
        }
    private:
        struct Cmp
        {
            Cmp() {}
            Cmp(const Cmp& src)
                : key(src.key), fn(src.fn)
            {
                static rapidjson::Value::AllocatorType alloc;
                to.CopyFrom(src.to, alloc, true);
            }
            std::string key;
            rapidjson::Value to;
            std::function<bool(const rapidjson::Value& /* src*/, const rapidjson::Value& /* dst */)> fn;
        };
        std::vector<Cmp> m_filter;
    };
    
    using Equal = EqualCondition<true, std::equal_to<rapidjson::Value>>;
    using NotEqual = EqualCondition<false, std::equal_to<rapidjson::Value>>;
    
    using Less = EqualCondition<true, std::less<rapidjson::Value>>;
    using LessOrEqual = EqualCondition<true, std::less_equal<rapidjson::Value>>;
    
    using Greater = EqualCondition<true, std::greater<rapidjson::Value>>;
    using GreaterOrEqual = EqualCondition<true, std::greater_equal<rapidjson::Value>>;
}

namespace std
{
    template<>
    constexpr bool less<rapidjson::Value>::operator()(const rapidjson::Value& left, const rapidjson::Value& right) const
    {
        if (left.GetType() != right.GetType())
            return false;
        if (left.IsUint64())
            return left.GetUint64() < right.GetUint64();

        if (left.IsUint())
            return left.IsUint() < right.IsUint();

        if (left.IsInt())
            return left.GetInt() < right.GetInt();

        if (left.IsDouble())
            return left.GetDouble() < right.GetDouble();

        if (left.IsFloat())
            return left.IsFloat() < right.IsFloat();

        if (left.IsFloat())
            return left.GetFloat() < right.GetFloat();

        if (left.IsBool())
            return left.GetBool() < right.GetBool();

        return left.GetString() < right.GetString();
    }

    template<>
    constexpr bool less_equal<rapidjson::Value>::operator()(const rapidjson::Value& left, const rapidjson::Value& right) const
    {
        if (left.GetType() != right.GetType())
            return false;
        if (left.IsUint64())
            return left.GetUint64() <= right.GetUint64();

        if (left.IsUint())
            return left.IsUint() <= right.IsUint();

        if (left.IsInt())
            return left.GetInt() <= right.GetInt();

        if (left.IsDouble())
            return left.GetDouble() <= right.GetDouble();

        if (left.IsFloat())
            return left.IsFloat() <= right.IsFloat();

        if (left.IsFloat())
            return left.GetFloat() <= right.GetFloat();

        if (left.IsBool())
            return left.GetBool() <= right.GetBool();

        return left.GetString() <= right.GetString();
    }

    template<>
    constexpr bool greater<rapidjson::Value>::operator()(const rapidjson::Value& left, const rapidjson::Value& right) const
    {
        if (left.GetType() != right.GetType())
            return false;
        if (left.IsUint64())
            return left.GetUint64() > right.GetUint64();

        if (left.IsUint())
            return left.IsUint() > right.IsUint();

        if (left.IsInt())
            return left.GetInt() > right.GetInt();

        if (left.IsDouble())
            return left.GetDouble() > right.GetDouble();

        if (left.IsFloat())
            return left.IsFloat() > right.IsFloat();

        if (left.IsFloat())
            return left.GetFloat() > right.GetFloat();

        if (left.IsBool())
            return left.GetBool() > right.GetBool();

        return left.GetString() > right.GetString();
    }

    template<>
    constexpr bool greater_equal<rapidjson::Value>::operator()(const rapidjson::Value& left, const rapidjson::Value& right) const
    {
        if (left.GetType() != right.GetType())
            return false;
        if (left.IsUint64())
            return left.GetUint64() >= right.GetUint64();

        if (left.IsUint())
            return left.IsUint() >= right.IsUint();

        if (left.IsInt())
            return left.GetInt() >= right.GetInt();

        if (left.IsDouble())
            return left.GetDouble() >= right.GetDouble();

        if (left.IsFloat())
            return left.IsFloat() >= right.IsFloat();

        if (left.IsFloat())
            return left.GetFloat() >= right.GetFloat();

        if (left.IsBool())
            return left.GetBool() >= right.GetBool();

        return left.GetString() >= right.GetString();
    }
}

#endif // JADB_EQUAL_CONDITION_H