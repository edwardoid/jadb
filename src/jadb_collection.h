#ifndef JADB_COLLECTION_H
#define JADB_COLLECTION_H

#include <memory>
#include <string>
#include <mutex>
#include <unordered_map>
#include <boost/filesystem.hpp>
#include "jadb_record.h"
#include "jadb_data_file.h"
#include "jadb_mapper.h"

namespace jadb
{
    class Collection: public std::enable_shared_from_this<Collection>
    {
    public:
        Collection(std::string name, boost::filesystem::path path, class Database*);
        std::string name() const;
        ~Collection();

        void lock();
        void unlock();

        uint64_t insert(Record& record);
        void remove(uint64_t id);
        bool contains(uint64_t id);
        Record get(uint64_t);
        uint32_t recordsPerFile() const;
    private:
        DataFile& bucket(uint32_t num);
    private:
        std::string m_name;
        boost::filesystem::path m_path;
        class Database* m_db;
        std::recursive_mutex m_mx;
        std::unordered_map < std::string, std::shared_ptr<DataFile>> m_files;
        Mapper<uint64_t> m_mapper;
    };
}

#endif // JADB_COLLECTION_H

