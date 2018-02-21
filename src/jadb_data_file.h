#ifndef JADB_DATA_FILE_H
#define JADB_DATA_FILE_H

#include "jadb_header.h"
#include "jadb_logger.h"
#include "jadb_serialization.h"

#include <memory>
#include <string>
#include <vector>

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem.hpp>

namespace jadb
{

	class DataFile
	{
	public:
		class Iterator{};
	public:
		DataFile(const boost::filesystem::path& path, std::shared_ptr<class Collection> collection);
		~DataFile();
		const Header& header() const;
		void recordAdded();
		void recordRemoved();
	protected:

		template<class T>
		void write(const T& obj, std::streampos offset)
		{
			{
			Logger::msg() << "Writing at " << (int)(offset);
			}
			m_file.seekp(offset, std::ios::beg);
			m_file.seekg(offset, std::ios::beg);
			Serialization oa(m_file);
			oa.serialize(obj);
			m_file.flush();
		}

		template<class T>
		void read(T& obj, std::streampos offset)
		{
			{
			Logger::msg() << "Reading at " << (int)(offset);
			}
			m_file.seekg(offset, std::ios::beg);
			m_file.seekp(offset, std::ios::beg);
			Serialization oa(m_file);
			oa.deserialize(obj);
		}

		bool checkSignature(uint32_t expected, std::streampos offset)
		{
			uint32_t sig = expected;
			read(sig, offset);
			return sig == expected;
		}

		void flush();

		friend class Collection;
	private:
		std::shared_ptr<class Collection> m_collection;
		boost::filesystem::path m_path;
		Header m_header;
		boost::filesystem::fstream m_file;
	};

}

#endif // JADB_DATA_FILE_H
