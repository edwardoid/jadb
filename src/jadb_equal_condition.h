#ifndef JADB_EQUAL_CONDITION_H
#define JADB_EQUAL_CONDITION_H

#include "jadb_condition.h"
#include <unordered_map>
#include <algorithm>

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

                if (ConditionBuilder::type(key) != ConditionBuilder::Type::Unsupported && i->value.IsObject())
                {
                    Condition* c = ConditionBuilder::create(key, i->value);
                    if (c == nullptr)
                        return false;
                    m_conditions.push_back(std::unique_ptr<Condition>(c));
                    continue;
                }

                const auto& src = i->value;
                auto& dst = m_filter[key];

                if(!copy<bool>(src, dst))
                    if(!copy<int>(src, dst))
                        if (!copy<unsigned int>(src, dst))
                            if (!copy<int64_t>(src, dst))
                                if (!copy<uint64_t>(src, dst))
                                    if (!copy<double>(src, dst))
                                        if (!copy<float>(src, dst))
                                            if (!copy<const char*>(src, dst))
                                            {
                                                return false;
                                            }
            }

            return true;
        }
        virtual bool exec() const override
        {
            return true;
        }
    private:

        template<typename T>
        inline bool copy(const rapidjson::Value& src, rapidjson::Value& dst)
        {
            if (src.Is<T>())
            {
                T val = src.Get<T>();
                dst.Set(val);
                return true;
            }

            return false;
        }
    private:
        std::unordered_map<std::string, rapidjson::Value> m_filter;
        std::vector<std::unique_ptr<Condition>> m_conditions;
    };

    struct EqualCmp
    {
        bool operator() () {}
    };

    struct LessCmp
    {
        bool operator() () {}
    };
    
    template<class P1, class P2>
    struct P1OrP2
    {
        P1 p1;
        P2 p2;
        bool operator() () { return p1() || p2(); }
    };


    using Equal = EqualCondition<true, EqualCmp>;
    using NotEqual = EqualCondition<false, EqualCmp>;
    
    using Less = EqualCondition<true, LessCmp>;
    
    using LessOrEqual = EqualCondition<true, P1OrP2<LessCmp, EqualCmp>>;
    
    using Greater = EqualCondition<false, P1OrP2<LessCmp, EqualCmp>>;

    using GreaterOrEqual = EqualCondition<false, LessCmp>;
}

#endif // JADB_EQUAL_CONDITION_H