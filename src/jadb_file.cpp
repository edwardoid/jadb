#include "jadb_file.h"

using namespace jadb;

File::File(boost::filesystem::path path, std::ios::openmode mode, bool open)
    : m_path(path), m_mode(mode)
{
    Lock lock(*this);
    if (open)
        this->open(mode);
}

const boost::filesystem::fstream& File::stream() const
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

void File::write(const char* data, size_t size)
{
    Lock lock(*this);
    m_stream.write(data, size);
}

void File::read(char* data, size_t size)
{
    Lock lock(*this);
    m_stream.read(data, size);
}
void File::flush()
{
    Lock lock(*this);
    m_stream.flush();
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

WriteTransaction::WriteTransaction(File& file, size_t bufferLength, size_t offset)
    : m_file(file)
{
    m_file.lock();
    if(bufferLength > 0)
        m_buff.reserve(bufferLength);
    if(offset > 0)
        m_file.seekForWrite(offset);
}

void WriteTransaction::write(const char* data, size_t len)
{
    m_buff.insert(m_buff.end(), data, data + len);
}

WriteTransaction::~WriteTransaction()
{
    if (!m_buff.empty())
    {
        m_file.write(m_buff.data(), m_buff.size());
        m_file.flush();
    }
    m_file.unlock();
}