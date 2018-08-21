#ifndef JADB_SEARCH_OPTIONS_H
#define JADB_SEARCH_OPTIONS_H

#include <nlohmann/json.hpp>

namespace jadb
{
    class SearchOptions
    {
    public:
        SearchOptions(const nlohmann::json& options);
        size_t getLimit() const;
        size_t getOffset() const;
        bool isSaved() const;
        bool canContinue() const;
        void increment();
        void decrement();
        void incrementBy(size_t count);
        void decrementBy(size_t count);
    private:
        size_t m_remain;
        size_t m_limit;
        size_t m_offset;
        bool m_saved;
    };
}

#endif // JADB_SEARCH_OPTIONS_H