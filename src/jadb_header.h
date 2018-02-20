#ifndef JADB_HEADER_H
#define JADB_HEADER_H

#include <stdint.h>
#include <memory>

#include "jadb_data_file.h"

namespace jadb
{
	class Header
	{
	public:
		Header(DataFile& owner);
		~Header();

		uint32_t begin() const;
		uint32_t end() const;
		uint32_t rows() const;

		static size_t length();
	protected:
		void read();
		void update();

		friend class DataFile;
	private:
		bool m_dirty;
		DataFile & m_ref;
		class HeaderData* m_data;
	};
}

#endif // JADB_DATA_FILE_H