#ifndef JADB_SYNCRONIZER_H
#define JADB_SYNCRONIZER_H

#include <functional>
#include <thread>
#include <chrono>
#include <mutex>
#include <vector>
#include <vector>
#include <atomic>

namespace jadb
{
    class Syncronizer: private std::thread
    {
    public:
        using SyncronizationTask = std::function<void()>;
    public:
        Syncronizer(size_t size = std::thread::hardware_concurrency() / 2);
        static int addOperation(SyncronizationTask& op);
        static void takeOperation(int id);
    private:
        static Syncronizer instance;

        void operator() ();
    private:
        std::vector<std::pair<int, SyncronizationTask>> m_tasks;
        std::mutex m_lock;
        int m_ids;
        size_t m_size;
    };
}


#endif // JADB_SYNCRONIZER_H