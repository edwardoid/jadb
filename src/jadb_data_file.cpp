#include "jadb_data_file.h"

#include <stdio.h>
#include <array>
#include "jadb_collection.h"
#include "jadb_configuration.h"
#include "jadb_filesystem.h"
#include "jadb_iterative_file.h"
using namespace jadb;

DataFile::DataFile(const boost::filesystem::path& path, Collection* collection)
    : IterativeFile(FileSystem::Get(path), SizeInfo<Header>::size()), m_path(path)
{
    OperationDuration op(Statistics::Type::FileIO);
    bool isNew = !boost::filesystem::exists(path);

    m_file = FileSystem::Get(path);

    Logger::msg() << (isNew ? "Creating new file " : "File found ") << path.generic_string();
    
    if(isNew)
    {
        m_file->close();
        m_file->open(std::ios::in | std::ios::out | std::ios::app);
        write<Header>(m_header, 0);
        
        char* empty = new char[Configuration::maxRecordSize()];
        memset(empty, ~Record::RecordSignature, sizeof(Record::RecordSignature));

        for(uint32_t i = 0; i < collection->recordsPerFile(); ++i)
        {
            m_file->write(empty, Configuration::maxRecordSize());
        }
        delete []empty;
        m_file->close();
        m_file->open();
    }
    else
    {
        read(m_header, 0);
    }
}

const Header& DataFile::header() const
{
    return m_header;
}

void DataFile::recordAdded()
{
    m_header.setRows(m_header.rows() + 1);
}

void DataFile::recordRemoved()
{
    m_header.setRows(m_header.rows() - 1);
}

void DataFile::flush()
{
    OperationDuration op(Statistics::Type::FileIO);
    m_file->flush();
}

DataFile::~DataFile()
{
    write(m_header, 0); // Update header
}
