#include "jadb_data_file.h"

#include <stdio.h>
#include <array>
#include "jadb_collection.h"
#include "jadb_filesystem.h"
using namespace jadb;

DataFile::DataFile(const boost::filesystem::path& path, std::shared_ptr<Collection> collection)
	: m_collection(collection), m_path(path)
{
	bool isNew = !boost::filesystem::exists(path);

    m_file = FileSystem::Get(path);
	
    Logger::msg() << (isNew ? "Creating new file " : "File found ") << path.generic_string();
	
	if(isNew)
	{
        m_file->close();
        m_file->open();
		write<Header>(m_header, 0);
		
		char* empty = new char[Record::MaxRecordSize];
		memset(empty, 0, Record::MaxRecordSize);
		for(uint32_t i = 0; i < m_collection->recordsPerFile(); ++i)
		{
			m_file->stream().write(empty, Record::MaxRecordSize);
			m_file->stream().flush();
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
	m_file->stream().flush();
}

DataFile::~DataFile()
{
	write(m_header, 0); // Update header
}
