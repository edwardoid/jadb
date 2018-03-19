#include "jadb_index_file.h"
#include "jadb_serialization.h"
#include "jadb_iterative_file.h"
#include "jadb_filesystem.h"
#include "jadb_stats.h"

namespace jadb
{
    struct Entry
    {
        uint32_t Value;
        uint64_t Id;
    };


    template<>
    inline void Serialization::serialize<Entry>(const Entry& obj)
    {
        Serialization::serialize(obj.Value);
        Serialization::serialize(obj.Id);
    }

    template<>
    inline void Serialization::deserialize(Entry& obj)
    {
        Serialization::deserialize(obj.Value);
        Serialization::deserialize(obj.Id);
    }

    template<>
    size_t SizeInfo<Entry>::size()
    {
        return sizeof(uint32_t) + sizeof(uint64_t);
    }
}

using namespace jadb;

IndexFile::IndexFile(const boost::filesystem::path& path)
    : m_file(FileSystem::Get(path))
{
    OperationDuration op(Statistics::Type::FileIO);
    m_file->close();
    m_file->open(std::ios::in | std::ios::binary);
    Serialization oa(m_file);
    oa.deserialize(m_name);
    oa.deserialize(m_index);
    m_headerEnd = m_file->readPosition();
    m_headerEnd = m_file->writePosition();
    m_file->close();
    m_file->open();
}

IndexFile::IndexFile(const boost::filesystem::path& path, std::string name, std::vector<std::string>& fields)
    : m_file(FileSystem::Get(path)), m_name(name), m_index(fields)
{
    OperationDuration op(Statistics::Type::FileIO);
    m_file->close();
    m_file->open(std::ios::out | std::ios::trunc | std::ios::binary);
    Serialization ia(m_file);
    ia.serialize(m_name);
    ia.serialize(m_index);
    m_headerEnd = m_file->writePosition();
    m_file->close();
    m_file->open();
}

void IndexFile::add(const Record& rec)
{
    OperationDuration op(Statistics::Type::IndexUpdate);
    Entry e;
    e.Value = m_index(rec);
    if (e.Value == m_index.CantCreateIndex)
        return;
    e.Id = rec.id();
    m_file->seekForWrite(m_file->size());
    Serialization s(m_file);
    s.serialize(e);
    m_file->flush();
}

std::vector<uint64_t> IndexFile::get(boost::property_tree::ptree& tree, size_t skip, size_t limit)
{
    OperationDuration op(Statistics::Type::SearchByIndex);
    std::vector<uint64_t> ids;
    auto filter = m_index(tree);

    if (filter == m_index.CantCreateIndex)
        return ids;

    m_file->seekForRead(m_headerEnd);
    IterativeFile<Entry> file(m_file, m_headerEnd);
    auto b = file.begin();
    auto e = file.end();
    while (skip > 0 && b != e)
    {
        auto elem = *b;
        if (elem.Value == filter)
        {
            skip--;
        }

        ++b;
    }

    while (b != e && ids.size() < limit)
    {
        auto elem = *b;
        if (elem.Value == filter)
            ids.push_back((*b).Id);
        ++b;
    }

    return ids;
}

IndexFile::~IndexFile()
{
    OperationDuration op(Statistics::Type::FileIO);
    m_file->flush();
}

void IndexFile::save()
{

}