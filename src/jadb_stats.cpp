#include "jadb_stats.h"
#include "jadb_logger.h"
#include <time.h>

using namespace jadb;

Statistics Statistics::m_instance;

Statistics::Statistics()
    : std::thread([&]() { report(); }), m_lastReport(clock())
{
    for (auto i = 0; i < (size_t)Type::Unknown; ++i)
        m_counts[i] = 0.0;
}

void Statistics::count(Type op, double count)
{
    if (op == Type::Unknown)
        op = Type::Other;
    m_instance.m_counts[(size_t)op] += count;
}

void Statistics::report()
{
    while (1)
    {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        double delta = (double)(clock() - m_instance.m_lastReport) / (double)(CLOCKS_PER_SEC);
        for (size_t i = 0; i <= (size_t)(Type::Unknown); ++i)
        {
            auto m = Logger::msg(1);
            double len = m_instance.m_counts[i] / delta;
            m_instance.m_counts[i] = 0;
            switch (static_cast<Type>(i))
            {
            case Type::FileIO:
            {
                m << "Average File I/O ";
                break;
            }
            case Type::Insert:
            {
                m << "Average insert: ";
                break;
            }
            case Type::Update:
            {
                m << "Average Remove: ";
                break;
            }
            case Type::Remove:
            {
                m << "Average Remove: ";
                break;
            }
            case Type::GetById:
            {
                m << "Average selecting by id: ";
                break;
            }
            case Type::IndexCreation:
            {
                m << "Average index creation: ";
                break;
            }
            case Type::IndexUpdate:
            {
                m << "Average Remove: ";
                break;
            }
            case Type::SearchByIndex:
            {
                m << "Average search by index: ";
                break;
            }
            case Type::Other:
            {
                m << "Average unlisted operation(aggregated): ";
                break;
            }
            }

            m << "took " << len << " sec.";
        }
        m_instance.m_lastReport = clock();
    }
}

OperationDuration::OperationDuration(Statistics::Type type)
    : m_start(clock()), m_type(type)
{}

OperationDuration::~OperationDuration()
{
    double delta = clock() - m_start;
    static const double cps = (double)(CLOCKS_PER_SEC);
    Statistics::count(m_type, delta / cps);
}