#ifndef JADB_FILE_REFERENCE_H
#define JADB_FILE_REFERENCE_H

#include <boost/filesystem/path.hpp>
#include <bo>
#include "jadb_serialization.h"


namespace jadb
{
    class FileReference
    {
    public:
        enum class Type
        {
            Database = 0,
            Collection = 1,
            Data = 2,
            Index = 3
        };
    public:
        FileReference(boost::filesystem::path path = "", Type type = Type::Database)
        {
            m_type = type;
            m_path = path;
        }
        ~FileReference() = default;

        boost::filesystem::path path();
        Type type();

    protected:
        friend class Serialization;
        Type m_type;
        boost::filesystem::path m_path;
    };

    template<>
    inline void Serialization::serialize<FileRefernce>(const FileReference& obj)
    {
        uint32_t type = static_cast<uint32_t>(obj.m_type);
        Serialization::serialize(type);
        auto str = obj.m_path.generic_string();
        Serialization::serialize(str);
    }

    template<>
    inline void Serialization::deserialize(FileReference& obj)
    {
        Serialization::deserialize(reinterpret_cast<uint32_t&>(obj.m_type));
        std::string str;
        Serialization::deserialize(str);
        obj.m_path = str;
    }
}

#endif // JADB_FILE_REFERENCE_H
