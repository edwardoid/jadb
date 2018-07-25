#include "jadb_file.h"
#include "jadb_logger.h"

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

size_t File::size()
{
    Lock lock(*this);
    return boost::filesystem::file_size(m_path);
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

std::ios::streampos File::readPosition()
{
    Lock lock(*this);
    return m_stream.tellg();
}

std::ios::streampos File::writePosition()
{
    Lock lock(*this);
    return m_stream.tellp();
}

void File::write(const char* data, size_t size)
{
    Lock lock(*this);
    m_stream.write(data, size);
    if(m_stream.bad())
    {
        Logger::err() << "Write operation failed. File: " << m_path.string();
    }
    m_dirty = true;
}

void File::read(char* data, size_t size)
{
    Lock lock(*this);
    if (m_dirty)
    {
        m_stream.flush();
        m_dirty = false;
    }
    m_stream.read(data, size);
    if(m_stream.bad())
    {
        Logger::err() << "Read operation failed. File: " << m_path.string();
    }
    else if(m_stream.gcount() != size)
    {
        Logger::err() << "Read operation failed. File: " << m_path.string() << " asked to read " << size << " got " << (size_t)m_stream.gcount();
    }
}
void File::flush()
{
    Lock lock(*this);
    m_stream.flush();
    m_dirty = false;
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
}