#include "jadb_data_file.h"

#include <stdio.h>
#include <array>
#include <boost/filesystem.hpp>
using namespace jadb;

DataFile::DataFile(const std::string& path)
	: m_path(path)
{
	bool isNew = !boost::filesystem::exists(path);	
	
	Logger::msg() << (isNew ? "Creating new file " : "File found ") << path;
	m_file.open(path, std::ios::in | std::ios::out | std::ios::binary | std::ios::app);
	
	if(isNew)
	{
		write<Header>(m_header, 0);
		
		/*char empty[5000000];
		for(int i = 0; i < 50; ++i)
		{
			m_file.write(empty, 5000000);
		}*/
		m_file.flush();
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

void DataFile::flush()
{
	m_file.flush();
}

DataFile::~DataFile()
{
}
