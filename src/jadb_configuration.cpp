#include "jadb_configuration.h"
#include "jadb_logger.h"
#include <boost/filesystem.hpp>
#include <nlohmann/json.hpp>

#define REST "rest"
#define REST_PORT "port"
#define REST_SECURE "secure"

#define DATA "data"
#define DATA_COMPRESSION "compression"
#define DATA_ROOT "root"

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

        auto restIt = cfg.find(REST);
        if(restIt != cfg.end() && restIt->is_object())
        {
            auto& rest = cfg[REST];
            if(rest.count(REST_PORT) != 0 && rest[REST_PORT].is_number_integer())
            {
                m_instance.m_port = static_cast<uint16_t>(rest[REST_PORT]);
            }

            if(rest.count(REST_SECURE) != 0 && rest[REST_SECURE].is_boolean())
            {
                m_instance.m_sslEnabled = static_cast<bool>(rest[REST_SECURE]);
            }
        }

        auto dataIt = cfg.find(DATA);
        if(dataIt != cfg.end() && dataIt->is_object())
        {
            auto& data = cfg[DATA];
            if(data.count(DATA_COMPRESSION) != 0 && data[DATA_COMPRESSION].is_boolean())
            {
                m_instance.m_compressionEnabled = static_cast<bool>(data[DATA_COMPRESSION]);
            }

            if(data.count(DATA_ROOT) != 0 && data[DATA_ROOT].is_string())
            {
                auto p = boost::filesystem::path(static_cast<std::string>(data[DATA_ROOT]));
                if(p.is_absolute())
                    m_instance.m_root = p;
                else
                    m_instance.m_root = boost::filesystem::absolute(p);
                
                boost::filesystem::create_directories(m_instance.m_root);
            }
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
        os << cfg.dump(4);
        os.flush();
        os.close();
    }
    catch (const std::exception& e)
    {
        Logger::err() << "Can't write configuration file: " << e.what();
    }
}