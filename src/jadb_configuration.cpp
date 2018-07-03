#include "jadb_configuration.h"
#include "jadb_logger.h"
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace jadb;

Configuration Configuration::m_instance;

void Configuration::load(boost::filesystem::path path)
{
    try
    {
        if(!boost::filesystem::exists(path.parent_path()))
            boost::filesystem::create_directories(path.parent_path());
        m_instance.m_path = path;
        m_instance.m_root = path.parent_path();
        boost::filesystem::ifstream is(path);
        if (is.good())
        {
            boost::property_tree::ptree cfg;
            boost::property_tree::read_json(is, cfg);
            m_instance.m_port = cfg.get<uint16_t>("rest.port", 8080);
            m_instance.m_sslEnabled = cfg.get<bool>("rest.secure", false);
            m_instance.m_compressionEnabled = cfg.get<bool>("data.compression", false);
            m_instance.m_root = cfg.get <std::string> ("data.root", path.parent_path().generic_string());
            boost::filesystem::create_directories(m_instance.m_root);
        }
        else
        {
            save();
            load(path);
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
        boost::filesystem::ofstream is(m_instance.m_path);
        boost::property_tree::ptree cfg;
            
        cfg.put<uint16_t>("rest.port", m_instance.m_port);
        cfg.put<bool>("rest.secure", m_instance.m_sslEnabled);
        cfg.put<bool>("data.compression", m_instance.m_compressionEnabled);
        cfg.put<uint32_t>("data.max_record_size", m_instance.m_maxDataSize);
        cfg.put<std::string>("data.root", m_instance.m_root.generic_string());
        boost::property_tree::write_json(is, cfg);
    }
    catch (const std::exception& e)
    {
        Logger::err() << "Can't write configuration file: " << e.what();
    }
}