#ifndef JADB_CONFIGURATION_H
#define JADB_CONFIGURATION_H

#include <stdint.h>
#include <boost/filesystem/path.hpp>

namespace jadb
{
    class Configuration
    {
    private:
        Configuration() = default;
        ~Configuration() = default;
    public:
        static void load(boost::filesystem::path path);
        static uint16_t port();
        static bool sslEnabled();
        static bool compressionEnabled();
        static uint32_t maxRecordSize();
        static boost::filesystem::path root();
        static void save();
    private:
        static Configuration m_instance;
        uint16_t m_port = 8080;
        bool m_sslEnabled = false;
        bool m_compressionEnabled = false;
        uint32_t m_maxDataSize = 5000000;
        boost::filesystem::path m_root;
        boost::filesystem::path m_path;
    };
}

#endif // JADB_CONFIGURATION_H