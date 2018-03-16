#include "jadb_filesystem.h"

using namespace jadb;

std::unordered_map < std::string, std::shared_ptr<File>> FileSystem::m_files;

FileSystem::FileSystem()
{
}


FileSystem::~FileSystem()
{
}

std::shared_ptr<File> FileSystem::Get(boost::filesystem::path file)
{
    auto it = m_files.find(file.generic_string());
    
    boost::filesystem::create_directories(file.parent_path());
    if (it == m_files.end())
    {
        m_files.insert({ file.generic_string(), std::make_shared<File>(file) });
    }
    return m_files[file.generic_string()];
}