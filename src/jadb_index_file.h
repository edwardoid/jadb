#ifndef JADB_INDEX_FILE_H
#define JADB_INDEX_FILE_H

#include <memory>
#include <string>
#include <vector>

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem.hpp>
#include <sparsepp/sparsepp/spp.h>

#include "jadb_file.h"
#include "jadb_index.h"
#include "jadb_record.h"

namespace jadb
{
    class IndexFile
    {
    public:
        IndexFile(const boost::filesystem::path& path);
        IndexFile(const boost::filesystem::path& path, std::string name, std::vector<std::string>& fields);
        ~IndexFile();
        void add(const Record& record);
        std::vector<uint64_t> get(boost::property_tree::ptree& query, size_t skip = 0, size_t limit = 999);
        const Index& index() const { return m_index; }
        const std::string& name() const { return m_name; }
    private:
        void save();
    private:
        
        spp::sparse_hash_map<uint32_t, spp::sparse_hash_set<uint64_t>> m_rows;

        std::shared_ptr<File> m_file;
        std::string m_name;
        size_t m_headerEnd;
        Index m_index;
    };
}

#endif // JADB_INDEX_FILE_H