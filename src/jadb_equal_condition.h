#ifndef JADB_EQUAL_CONDITION_H
#define JADB_EQUAL_CONDITION_H

#include "jadb_collection.h"
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
        virtual bool create(const nlohmann::json& obj) override
        {
            if (!obj.is_object())
            {
                return false;
            }
            for (nlohmann::json::const_iterator i = obj.begin(); i != obj.end(); ++i)
            {
                if (i.key().empty())
                {
                    return false;
                }

                const auto& src = *i;

                if (src.is_object())
                {
                    bool hasKey = false;
                    ConditionBuilder::Type type = ConditionBuilder::type(src, hasKey);
                    if (!hasKey)
                        return false;
                    auto& val = *(src.begin());
                    auto& key = src.begin().key();
                    switch (type)
                    {
                        case ConditionBuilder::Type::Less:
                        {
                            createFilter<std::less<nlohmann::json>>(key, val);
                            break;
                        }
                        case ConditionBuilder::Type::LessOrEqual:
                        {
                            createFilter<std::less_equal<nlohmann::json>>(key, val);
                            break;
                        }
                        case ConditionBuilder::Type::Greater:
                        {
                            createFilter<std::greater<nlohmann::json>>(key, val);
                            break;
                        }
                        case ConditionBuilder::Type::GreaterOrEqual:
                        {
                            createFilter<std::greater_equal<nlohmann::json>>(key, val);
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
                    createFilter<Predicate>(i.key(), src);
                }
            }

            return true;
        }
        virtual bool exec(const Collection& record, btree::btree_set<uint64_t>* filter) const override
        {
            return true;
        }
    private:

        template<typename BinaryPredicate>
        void createFilter(const std::string& key, const nlohmann::json& src)
        {
            Cmp c;
            c.key = key;
            c.to = src;
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
                to = src.to;
            }
            std::string key;
            nlohmann::json to;
            std::function<bool(const nlohmann::json& /* src*/, const nlohmann::json& /* dst */)> fn;
        };
        std::vector<Cmp> m_filter;
    };
    
    using Equal = EqualCondition<true, std::equal_to<nlohmann::json>>;
    using NotEqual = EqualCondition<false, std::equal_to<nlohmann::json>>;
    
    using Less = EqualCondition<true, std::less<nlohmann::json>>;
    using LessOrEqual = EqualCondition<true, std::less_equal<nlohmann::json>>;
    
    using Greater = EqualCondition<true, std::greater<nlohmann::json>>;
    using GreaterOrEqual = EqualCondition<true, std::greater_equal<nlohmann::json>>;
}

namespace std
{
    template<>
    bool less<nlohmann::json>::operator()(const nlohmann::json& left, const nlohmann::json& right) const
    {
        return left < right;
    }

    template<>
    bool less_equal<nlohmann::json>::operator()(const nlohmann::json& left, const nlohmann::json& right) const
    {
        return left <= right;
    }

    template<>
    bool greater<nlohmann::json>::operator()(const nlohmann::json& left, const nlohmann::json& right) const
    {
       return left > right;
    }

    template<>
    bool greater_equal<nlohmann::json>::operator()(const nlohmann::json& left, const nlohmann::json& right) const
    {
       return left >= right;
    }
}

#endif // JADB_EQUAL_CONDITION_H
