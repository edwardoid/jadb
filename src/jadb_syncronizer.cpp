#include "jadb_syncronizer.h"
#include <vector>
#include <algorithm>

using namespace jadb;

Syncronizer Syncronizer::instance;

int Syncronizer::addOperation(Syncronizer::SyncronizationTask& task)
{
    std::lock_guard<std::mutex> guard(instance.m_lock);
    instance.m_tasks.emplace_back(std::make_pair(instance.m_ids, std::move(task)));
    return instance.m_ids++;
}

void Syncronizer::takeOperation(int id) {
    std::lock_guard<std::mutex> guard(instance.m_lock);
    auto it = std::lower_bound(instance.m_tasks.begin(), instance.m_tasks.end(), id, [](auto& l, auto id) { return l.first < id;  });
    if (it == instance.m_tasks.end() || it->first != id)
        return;
    instance.m_tasks.erase(it);
}

Syncronizer::Syncronizer(size_t size)
    : m_ids(1), thread([this]() { (*this)(); })
{
    m_size = size == 0 ? 1 : size;
}

void Syncronizer::operator()()
{
    while (1)
    {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        std::atomic<size_t> next(0);
        std::lock_guard<std::mutex> guard(m_lock);
        std::vector<std::thread> threads;
        size_t s = 0;
        while (s++ < m_size)
        {
            threads.push_back(std::thread([&]() {
                size_t i = next.fetch_add(1);
                while (i < m_tasks.size())
                {
                    m_tasks[i].second();
                    i = next.fetch_add(1);
                }
            }));
        }

        s = 0;
        while (s < m_size)
        {
            threads[s++].join();
        }
    }
}