#include "jadb.h"
#include "jadb_logger.h"


using namespace jadb;


Database::Database(std::string root)
    : m_path(root)
{
    m_path = boost::filesystem::canonical(m_path);
    boost::filesystem::directory_iterator it(root), end;
    {
        auto msg = Logger::msg();
        msg << "Looking for colelctions in " << m_path.generic_string();
    }
    for (; it != end; ++it)
    {
        boost::filesystem::path path = it->path();
        if (boost::filesystem::is_directory(path))
        {
            auto name = path.filename().generic_string();
            auto msg = Logger::msg();
            msg << "Found collection " << name;
            auto collection = std::make_shared<Collection>(name, path, this);
            m_collection.emplace(std::make_pair(name, collection));
        }
    }
    
}

Database::~Database()
{
}

std::unordered_map<std::string, std::shared_ptr<Collection>>& Database::collections()
{
    return m_collection;
}

Collection& Database::operator[] (const std::string name)
{
    return *(m_collection[name]);
}

bool Database::has(std::string collection) const
{
    return m_collection.find(collection) != m_collection.cend();
}

Collection& Database::create(std::string collection)
{
    if (has(collection))
    {
        return *(m_collection[collection]);
    }

    auto path = m_path;
    path.append(collection);

    if (!boost::filesystem::create_directory(path))
    {
        Logger::err() << "Can't create collection " << path.generic_string();
        throw std::runtime_error("Creating directory failed");
    }
    Logger::msg() << "Creating collection " << collection << " as " << path.generic_string();
    m_collection.insert(std::make_pair(collection, std::make_unique<Collection>(collection, path, this)));
    return *(m_collection[collection]);
}
