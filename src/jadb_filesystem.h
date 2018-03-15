#ifndef JADB_FILESYSTEM_H
#define JADB_FILESYSTEM_H

#include <unordered_map>
#include <string>
#include <memory>
#include "jadb_file.h"
#include <boost\filesystem\path.hpp>

namespace jadb
{
	class FileSystem
	{
	public:
		FileSystem();
		~FileSystem();
		static std::shared_ptr<File> Get(boost::filesystem::path file);
	private:
		static std::unordered_map<std::string, std::shared_ptr<File>> m_files;
	};
}

#endif // JADB_FILESYSTEM_H