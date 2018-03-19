#ifndef JADB_STATS_H
#define JADB_STATS_H

#include <string>
#include <atomic>
#include <mutex>
#include <time.h>
#include <thread>

namespace jadb
{
    class Statistics: private std::thread
    {
    public:
        enum class Type
        {
            FileIO,
            Insert,
            Update,
            Remove,
            GetById,
            IndexCreation,
            IndexUpdate,
            SearchByIndex,
            Other,
            Unknown,
        };

        Statistics();
        ~Statistics() = default;


        static void count(Type operation, double count);
        static void report();
    private:
        static Statistics m_instance;
        clock_t m_lastReport;
        double m_counts[(size_t)Type::Unknown];
    };


    class OperationDuration
    {
    public:
        OperationDuration(Statistics::Type type);
        ~OperationDuration();
    private:
        clock_t m_start;
        Statistics::Type m_type;
    };
}

#endif // JADB_STATS_H