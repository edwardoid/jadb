#ifndef JADB_SPARSEPP_ADAPTORS_H
#define JADB_SPARSEPP_ADAPTORS_H

#include "jadb_file.h"
#include <sparsepp/sparsepp/spp.h>

namespace jadb
{
    namespace sparsepp
    {
        struct FilePointer
        {
            FilePointer(File* file) : m_fp(file) {}

            size_t Write(const void* data, size_t length)
            {
                if (m_fp != nullptr)
                {
                    m_fp->write(reinterpret_cast<const char*>(data), length);
                    return length;
                }

                return 0;
            }
        private:
            File * m_fp = nullptr;
        };

        template<typename KeyT, typename ValueT, class KeySerializerC = Serialization, class ValueSerializerC = Serialization>
        class MapValueSerializator
        {
        public:
            MapValueSerializator(std::shared_ptr<File>& file) : m_file(file) {}
            MapValueSerializator(const MapValueSerializator& src) : m_file(src.m_file) {}
            bool operator() (FilePointer* fp, const std::pair<KeyT, ValueT>& pair)
            {
                KeySerializerC k(m_file);
                k.serialize(pair.first);

                ValueSerializerC v(m_file);
                v.serialize(const_cast<ValueT&>(pair.second));
                return true;
            }

        private:
            std::shared_ptr<File> m_file;
        };

        template<typename ValueT, class ValueSerializerC = Serialization>
        class ValueSerializator
        {
        public:
            ValueSerializator(std::shared_ptr<File>& file) : m_file(file) {}
            ValueSerializator(const ValueSerializator& src) : m_file(src.m_file) {}
            bool operator() (FilePointer* fp, const ValueT& value)
            {
                ValueSerializerC v(m_file);
                v.serialize(const_cast<ValueT&>(value));
                return true;
            }

        private:
            std::shared_ptr<File> m_file;
        };

        template<typename Key, typename Value, typename KeySerializer = Serialization, typename ValueSerializer = Serialization>
        class SparseppMapSerializator
        {
        public:
            SparseppMapSerializator(std::shared_ptr<File> stream)
                : m_stream(stream)
            {}
            ~SparseppMapSerializator() = default;

            void serialize(spp::sparse_hash_map<Key, Value>& map)
            {
                FilePointer fp(m_stream.get());
                map.serialize(MapValueSerializator<Key, Value, KeySerializer, ValueSerializer>(m_stream), &fp);
            }

            void deserialize(const spp::sparse_hash_map<Key, Value>& map)
            {
                //Serialization(m_stream).deserialize<Type>(*(obj.get()));
            }
        protected:
            std::shared_ptr<File> m_stream;
        };

        template<typename Value, typename ValueSerializerC = Serialization>
        class SparseppSetSerializator
        {
        public:
            SparseppSetSerializator(std::shared_ptr<File> stream)
                : m_stream(stream)
            {}
            ~SparseppSetSerializator() = default;

            void serialize(spp::sparse_hash_set<Value>& set)
            {
                FilePointer fp(m_stream.get());
                set.serialize(ValueSerializator<Value, ValueSerializerC>(m_stream), &fp);
            }

            void deserialize(spp::sparse_hash_set<Value>& set)
            {
                //Serialization(m_stream).deserialize<Type>(*(obj.get()));
            }
        protected:
            std::shared_ptr<File> m_stream;
        };
    }
}


#endif // JADB_SPARSEPP_ADAPTORS_H