#ifndef JADB_MAPPER_H
#define JADB_MAPPER_H

#include <stdint.h>
#include <unordered_map>

namespace jadb
{
	template<typename KeyT, class Hash = std::hash<KeyT>>
	class Mapper
	{
	public:
		struct Position
		{
			uint32_t Bucket;
			uint32_t Offset;
		};
	public:
		Mapper(uint32_t buckets = 32, uint32_t capacity = 528, Hash hasher = Hash())
			: m_buckets(buckets), m_capacity(capacity), m_hasher(hasher)
		{}
		Mapper(const Mapper& src) { *this = src; }
		const Mapper& operator = (const Mapper& src) { m_buckets = src.m_buckets; m_capacity = src.m_capacity; m_hasher = src.m_hasher; return *this; }
		Position operator[](const KeyT key) { return find(key); }
		Position find(KeyT key);
		uint32_t buckets() const { return m_buckets; }
		uint32_t capacity() const { return m_capacity; }
	private:
		uint32_t m_buckets;
		uint32_t m_capacity;
		Hash m_hasher;
	};


	template<typename KeyT, class Hash>
	typename Mapper<KeyT, Hash>::Position Mapper<KeyT, Hash>::find(KeyT key)
	{
		uint32_t val = static_cast<uint32_t>(m_hasher(key));
		val = key;
		Position pos;
		pos.Bucket = val / m_capacity;
		pos.Offset = val % m_buckets;
		return pos;
	}
}

#endif // JADB_MAPPER_H
