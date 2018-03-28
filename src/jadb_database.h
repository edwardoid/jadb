#ifndef JADB_DATABASE_H
#define JADB_DATABASE_H

#include<string>
#include<memory>
#include<unordered_map>
#include <boost/filesystem.hpp>

#include "jadb_collection.h"

namespace jadb
{
    class Collection;
    class Database
    {
    public:
        Database(std::string name);
        ~Database();
        std::unordered_map<std::string, std::shared_ptr<Collection>>& collections();
        Collection& operator[] (const std::string name);
        bool has(std::string colelction) const;
        Collection& create(std::string collection);
    private:
        std::unordered_map<std::string, std::shared_ptr<Collection>> m_collection;
        boost::filesystem::path m_path;
    };
}

#endif // JADB_DATABASE_H