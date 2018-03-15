#ifndef JADB_FILE_H
#define JADB_FILE_H

#include <mutex>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

namespace jadb
{
    class File
    {
    public:
        File(boost::filesystem::path path, std::ios::openmode mode = std::ios::in | std::ios::out | std::ios::binary, bool open = true);
        ~File();
        const boost::filesystem::fstream& stream() const;
        const boost::filesystem::path& path() const;
        size_t size();
        void open(std::ios::openmode mode = std::ios::in | std::ios::out | std::ios::binary);
        void close();
        bool opened() const;
        bool closed() const;
        void seekForRead(std::streampos pos);
        void seekForWrite(std::streampos pos);
        void write(const char* data, size_t size);
        void read(char* data, size_t size);
        void flush();
        void lock();
        void unlock();

        class Lock
        {
        public:
            Lock(const File& file) : m_ref(const_cast<File&>(file)) { m_ref.lock(); }
            ~Lock() { m_ref.unlock(); }
        private:
            File & m_ref;
        };
    private:
        std::ios::openmode m_mode;
        boost::filesystem::fstream m_stream;
        boost::filesystem::path m_path;
        std::recursive_mutex m_mx; // Allow locking in the same thread multiple times
    };

    class WriteTransaction
    {
    public:
        WriteTransaction(File& file, size_t bufferLength = 512, size_t offset = 0);
        void write(const char* data, size_t len);
        ~WriteTransaction();
    private:
        std::vector<char> m_buff;
        File& m_file;
    };
}

#endif // JADB_FILE_H