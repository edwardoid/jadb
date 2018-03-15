#include "jadb_file.h"

using namespace jadb;

File::File(boost::filesystem::path path, std::ios::openmode mode, bool open)
	: m_path(path), m_mode(mode)
{
	Lock lock(*this);
	if (open)
		this->open(mode);
}

boost::filesystem::fstream& File::stream()
{
    return m_stream;
}

void File::open(std::ios::openmode mode)
{
	Lock lock(*this);
	if (m_stream.is_open())
	{
		m_stream.close();
	}
	m_stream.open(m_path, mode);
}

void File::close()
{
	Lock lock(*this);
	if (m_stream.is_open())
		m_stream.close();
}

bool File::opened() const
{
	Lock lock(*this);
	return m_stream.is_open();
}

bool File::closed() const
{
	return !opened();
}

void File::seekForRead(std::ios::streampos pos)
{
	Lock lock(*this);
	if (m_stream.is_open())
		m_stream.seekg(pos);
}

void File::seekForWrite(std::ios::streampos pos)
{
	Lock lock(*this);
	if (m_stream.is_open())
		m_stream.seekp(pos);
}

void File::lock()
{
	m_mx.lock();
}

void File::unlock()
{
	m_mx.unlock();
}

File::~File()
{

}
