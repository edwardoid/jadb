#include "jadb_configuration.h"
#include "jadb_logger.h"
#include <boost/filesystem.hpp>
#include <nlohmann/json.hpp>

#define REST_PORT "rest.port"
#define REST_SECURE "rest.secure"

#define DATA_COMPRESSION "data.compression"
#define DATA_ROOT "data.root"

using namespace jadb;

Configuration Configuration::m_instance;

void Configuration::load(boost::filesystem::path path)
{
    try
    {
        path = boost::filesystem::absolute(path);
        Logger::msg() << "Loading configuration from " << path.string();
        m_instance.m_path = path;
        m_instance.m_root = path.parent_path();
        Logger::msg() << "Loading configuration from " << path.string();
        boost::filesystem::ifstream is(path);

        std::string json;
        while(is.good()) {
            std::string tmp;
            is >> tmp;
            json += tmp;
        }


        auto cfg = nlohmann::json::parse(json);

        m_instance.m_port = 8080;
        m_instance.m_sslEnabled = false;
        m_instance.m_compressionEnabled = false;
        m_instance.m_root = boost::filesystem::current_path();

        if(cfg.find(REST_PORT) != cfg.end())
        {
            auto val = cfg[REST_PORT];
            if(val.is_number_integer())
            {
                m_instance.m_port = static_cast<uint16_t>(val);
            }
        }


        if(cfg.find(REST_SECURE) != cfg.end())
        {
            auto val = cfg[REST_SECURE];
            if(val.is_boolean())
            {
                m_instance.m_sslEnabled = static_cast<bool>(val);
            }
        }

        if(cfg.find(DATA_COMPRESSION) != cfg.end())
        {
            auto val = cfg[DATA_COMPRESSION];
            if(val.is_boolean())
            {
                m_instance.m_compressionEnabled = static_cast<bool>(val);
            }
        }

        if(cfg.find(DATA_ROOT) != cfg.end())
        {
            auto val = cfg[DATA_ROOT];
            if(val.is_string())
            {
                m_instance.m_root = boost::filesystem::absolute(boost::filesystem::path(val.dump()));
            }
            boost::filesystem::create_directories(m_instance.m_root);
        }
    }
    catch (const std::exception& e)
    {
        Logger::err() << "Can't write configuration file: " << e.what();
    }
}

uint16_t Configuration::port()
{
    return m_instance.m_port;
}

bool Configuration::compressionEnabled()
{
    return m_instance.m_compressionEnabled;
}

bool Configuration::sslEnabled()
{
    return m_instance.m_sslEnabled;
}

uint32_t Configuration::maxRecordSize()
{
    return m_instance.m_maxDataSize;
}

boost::filesystem::path Configuration::root()
{
    return m_instance.m_root;
}

void Configuration::save()
{
    try
    {
        nlohmann::json cfg;
        boost::filesystem::ofstream os(m_instance.m_path);
            
        cfg["rest"]["port"] = m_instance.m_port;
        cfg["rest"]["secure"] = m_instance.m_sslEnabled;
        cfg["data"]["compression"] = m_instance.m_compressionEnabled;
        cfg["data"]["max_record_size"] = m_instance.m_maxDataSize;
        cfg["data"]["root"] = m_instance.m_root.string();
        os << cfg.dump();
        os.flush();
        os.close();
    }
    catch (const std::exception& e)
    {
        Logger::err() << "Can't write configuration file: " << e.what();
    }
}