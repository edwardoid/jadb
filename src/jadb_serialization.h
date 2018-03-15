#ifndef JADB_SERIALIZATION_H
#define JADB_SERIALIZATION_H

#include "jadb_file.h"
#include <memory>
#include <vector>

namespace jadb
{
	class Serialization
	{
	public: 
		Serialization(std::shared_ptr<File> stream)
			: m_stream(stream) {}
		~Serialization() = default;

		template<typename T>
		void serialize(const T& obj)
		{
			m_stream->write(reinterpret_cast<const char*>(&obj), sizeof(obj));
		}

		template<typename T>
		void deserialize(T& obj)
		{
			m_stream->read(reinterpret_cast<char*>(&obj), sizeof(obj));
		}
	private:
		std::shared_ptr<File> m_stream;
	};

	template<>
	inline void Serialization::serialize(const std::string& obj)
	{
		Serialization len(m_stream);
		len.serialize(static_cast<uint32_t>(obj.size()));
		m_stream->write(obj.data(), obj.size());
	}

	template<>
	inline void Serialization::deserialize(std::string& obj)
	{
		Serialization len(m_stream);
		uint32_t sz = 0;
		len.deserialize(sz);
		obj.resize(sz);
		if (sz > 0)
		{
			m_stream->read(const_cast<char*>(obj.data()), sz);
		}
	}
}

#endif // JADB_SERIALIZATION_H