#include "jadb_index_file.h"
#include "jadb_serialization.h"
#include "jadb_iterative_file.h"
#include "jadb_filesystem.h"
#include "jadb_stats.h"
#include "jadb_sparsepp_adaptors.h"

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

    m_rows[e.Value].insert(e.Id);

    sparsepp::SparseppMapSerializator<uint32_t,
                                      spp::sparse_hash_set<uint64_t>,
                                      Serialization,
                                      sparsepp::SparseppSetSerializator<uint64_t>> ser(m_file);
    ser.serialize(m_rows);

    m_file->flush();

    m_file->seekForWrite(m_file->size());
    Serialization s(m_file);
    s.serialize(e);
    m_file->flush();
}

std::vector<uint64_t> IndexFile::get(const nlohmann::json& tree, size_t skip, size_t limit)
{
    OperationDuration op(Statistics::Type::SearchByIndex);
    std::vector<uint64_t> ids;
    auto filter = m_index(tree);

    if (filter == m_index.CantCreateIndex)
        return ids;

    if (!m_rows.contains(filter))
        return ids;

    auto& rows = m_rows.at(filter);

    auto b = rows.cbegin();
    auto e = rows.cend();
    while (skip-- > 0 && b != e)
    {
        ++b;
    }

    while (limit-- > 0 && b != e)
    {
        ids.push_back(*b);
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