#ifndef JADB_SERIALIZATION_H
#define JADB_SERIALIZATION_H

#include "jadb_file.h"
#include <memory>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <boost/endian/conversion.hpp>

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
    protected:
        class MapSerializer;
        class Array;
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

    class NumberSerialization
    {
    public:
        NumberSerialization(std::shared_ptr<File> stream)
            : m_stream(stream)
        {}
        ~NumberSerialization() = default;
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
    protected:
        std::shared_ptr<File> m_stream;
    };

    class MapSerialization
    {
    public:
        MapSerialization(std::shared_ptr<File> stream)
            : m_stream(stream)
        {}
        ~MapSerialization() = default;

        template<typename K, typename V, class Container = std::map<K, V>, class KeySerializer = Serialization, class ValueSerilizer = Serialization>
        void serialize(const Container& obj)
        {
            Serialization(m_stream).serialize<size_t>(obj.size());
            for (auto& kp : obj)
            {
                KeySerializer(m_stream).serialize(kp.first);
                ValueSerilizer(m_stream).serialize(kp.second);
            }
        }

        template<typename K, typename V, class Container = std::map<K, V>, class KeySerializer = Serialization, class ValueSerilizer = Serialization>
        void deserialize(Container& obj)
        {
            size_t sz = 0;
            Serialization(m_stream).deserialize<size_t>(sz);
            while (sz--)
            {
                K key;
                V value;
                KeySerializer(m_stream).deserialize(key);
                ValueSerilizer(m_stream).deserialize(value);
                obj.insert(std::make_pair(key, value));
            }
        }
    protected:
        std::shared_ptr<File> m_stream;
    };

    class ArraySerialization
    {
    public:
        ArraySerialization(std::shared_ptr<File> stream)
            : m_stream(stream)
        {}
        ~ArraySerialization() = default;

        template<typename V, class Container = std::vector<V>>
        void serialize(const Container& obj)
        {
            const uint32_t sz = static_cast<uint32_t>(obj.size());
            Serialization(m_stream).serialize<uint32_t>(sz);
            for (auto& val : obj)
            {
                Serialization(m_stream).serialize<V>(val);
            }
        }

        template<typename V, class Container = std::vector<V>>
        void deserialize(Container& obj)
        {
            uint32_t sz = 0;
            Serialization(m_stream).deserialize<uint32_t>(sz);
            while (sz--)
            {
                V value;
                Serialization(m_stream).deserialize<V>(value);
                obj.push_back(value);
            }
        }
    protected:
        std::shared_ptr<File> m_stream;
    };

    template<typename Type>
    class SmartPointerSerialization
    {
    public:
        SmartPointerSerialization(std::shared_ptr<File> stream)
            : m_stream(stream)
        {}
        ~SmartPointerSerialization() = default;

        template<typename Pointer = std::shared_ptr<Type>>
        void serialize(const Pointer& obj)
        {
            Serialization(m_stream).serialize<Type>(*(obj.get()));
        }

        template<typename Pointer = std::shared_ptr<Type>>
        void deserialize(Pointer& obj)
        {
            Serialization(m_stream).deserialize<Type>(*(obj.get()));
        }
    protected:
        std::shared_ptr<File> m_stream;
    };
}

#endif // JADB_SERIALIZATION_H
