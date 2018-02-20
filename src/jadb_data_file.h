#ifndef JADB_DATA_FILE_H
#define JADB_DATA_FILE_H

#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace jadb
{

	class DataFile
	{
	public:
		class Iterator{};
	public:

		DataFile(const std::string& path);
		~DataFile();
		size_t size();
	protected:
		size_t pos();
		void seek(size_t pos);
		void dataSeek(size_t pos);
		void read(uint8_t* buff, size_t len);
		void read(std::vector<uint8_t>& data);

		void write(const uint8_t* buff, size_t len);
		void write(const std::vector<uint8_t>& data);

		void flush();

		friend class Header;
		friend class Collection;
	private:
		std::unique_ptr<class Header> m_header;
		std::fstream m_file;
	};

}

#endif // JADB_DATA_FILE_H