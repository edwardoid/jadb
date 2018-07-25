#ifndef JADB_COLLECTION_H
#define JADB_COLLECTION_H

#include <memory>
#include <vector>
#include <string>
#include <mutex>
#include <unordered_map>
#include <boost/filesystem.hpp>
#include "jadb_record.h"
#include "jadb_data_file.h"
#include "jadb_index_file.h"
#include "jadb_mapper.h"
#include "jadb_id_mapping.h"
#include "jadb_query.h"

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

        bool hasIndex(const std::string& name) const;

        uint64_t insert(Record& record);
        void remove(uint64_t id);
        bool contains(uint64_t id);
        Record get(uint64_t id, bool fast = true) const;
        uint32_t recordsPerFile() const;
        void createIndex(std::string name, std::vector<std::string>& properties);
        std::vector<Record> searchByIndex(std::string index, std::unordered_map<std::string, std::string>& filter, size_t limit = 999, size_t skip = 0);
        std::vector<Record> query(const Query& query);
        IdMapping::ConstIterator recordsBegin() const;
        IdMapping::ConstIterator recordsEnd() const;
    private:
        std::shared_ptr<DataFile> bucket(uint32_t num, bool create) const;
        void removeBucket(uint32_t num);
        boost::filesystem::path indexFilePath(const std::string& name) const;
    private:
        std::string m_name;
        boost::filesystem::path m_path;
        boost::filesystem::path m_dataDir;
        boost::filesystem::path m_indicesDir;
        std::atomic<uint64_t> m_ids;
        class Database* m_db;
        std::recursive_mutex m_mx;
        mutable std::unordered_map < std::string, std::shared_ptr<DataFile>> m_data;
        std::unordered_map < std::string, std::shared_ptr<IndexFile>> m_indices;
        IdMapping m_records; // Mapping ID -> hash
        Mapper<uint64_t> m_mapper;
    };
}

#endif // JADB_COLLECTION_H

