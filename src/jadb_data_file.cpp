#include "jadb_data_file.h"
#include "jadb_header.h"

using namespace jadb;

DataFile::DataFile(const std::string& path)
{
	m_file.open(path, std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::app);
	m_header = std::make_unique<Header>(*this);
	if (size() < Header::length())
	{
		m_header->update();
	}	
}

size_t DataFile::size()
{
	auto pos = m_file.tellg();


	auto begin = m_file.tellg();
	m_file.seekg(0, std::ios::end);
	auto end = m_file.tellg();

	m_file.seekg(pos);
	return (end - begin);
}


size_t DataFile::pos()
{
	return m_file.tellg();
}

void DataFile::seek(size_t pos)
{
	m_file.seekp(pos, std::ios::beg);
	m_file.seekg(pos, std::ios::beg);
}

void DataFile::dataSeek(size_t pos)
{
	seek(pos + Header::length());
}

void DataFile::read(uint8_t* buff, size_t len)
{
	m_file.read(reinterpret_cast<char*>(buff), len);
}

void DataFile::read(std::vector<uint8_t>& data)
{
	m_file.read(reinterpret_cast<char*>(buff), len);
}

void DataFile::write(const uint8_t* buff, size_t len)
{
	m_file.write(reinterpret_cast<const char*>(buff), len);
}

void DataFile::flush()
{
	m_file.flush();
}

DataFile::~DataFile()
{
}
