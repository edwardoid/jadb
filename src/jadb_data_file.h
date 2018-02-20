#ifndef JADB_DATA_FILE_H
#define JADB_DATA_FILE_H

#include "jadb_header.h"
#include "jadb_logger.h"

#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>


namespace jadb
{

	class DataFile
	{
	public:
		class Iterator{};
	public:
		using Input = boost::archive::text_iarchive;
		using Output = boost::archive::text_oarchive;
		DataFile(const std::string& path);
		~DataFile();
		const Header& header() const;
		
	protected:

		template<class T>
		void write(const T& obj, std::streampos offset)
		{
			{
			Logger::msg() << "Writing at " << (int)(offset);
			}
			m_file.seekp(offset, std::ios::beg);
			Output oa(m_file);
			(oa) & obj;
			m_file.flush();
		}

		template<class T>
		void read(T& obj, std::streampos offset)
		{
			{
			Logger::msg() << "Reading at " << (int)(offset);
			}
			Input ia(m_file);
			m_file.seekg(offset, std::ios::beg);
			(ia) & obj;
		}

		void flush();

		friend class Collection;
	private:
		std::string m_path;
		std::shared_ptr<Input> m_ia;
		std::shared_ptr<Output> m_oa;
		Header m_header;
		std::fstream m_file;
	};

}

#endif // JADB_DATA_FILE_H
