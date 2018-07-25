#include "jadb_logger.h"
#include <chrono>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>

#pragma warning(disable : 4996)

using namespace jadb;

Logger& Logger::log()
{
    static Logger instance;
    return instance;
}

void Logger::write(const std::string& str)
{
    std::cout << str;
}

Logger::Message Logger::msg(uint8_t ident)
{
    Message m(log());
    m.ident(ident);
    return m;
}

Logger::Error Logger::err(uint8_t ident)
{
    Error m(log());
    m.ident(ident);
    return m;
}

Logger::Debug Logger::dbg(uint8_t ident)
{
    Debug m(log());
    m.ident(ident);
    return m;
}

Logger::Message::Message(Logger& instance, std::string level)
    : m_instance(instance), m_level(level)
{}

Logger::Message& Logger::Message:: operator << (const int& val) { m_ss << val; return *this; }
Logger::Message& Logger::Message:: operator << (const uint32_t& val) { m_ss << val; return *this; }
Logger::Message& Logger::Message:: operator << (const size_t& val) { m_ss << val; return *this; }
Logger::Message& Logger::Message:: operator << (const double& val) { m_ss << val; return *this; }
Logger::Message& Logger::Message:: operator << (const std::string& msg) { m_ss << msg; return *this; }

void Logger::Message::ident(uint8_t ident)
{
    m_identCount = ident;
}

void Logger::Message::end()
{
    auto str = m_ss.str();
    if (str.empty())
        return;

    m_ss.str(std::string());
    m_ss.clear();
    
    const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();

    m_ss << boost::posix_time::to_simple_string(now) << " [" << m_level << "] : ";
    while (m_identCount--)
        m_ss << '\t';
    if (m_level == "ERR")
        m_ss << "\033[1;31";
    else if (m_level == "DBG")
        m_ss << "\033[1;33";
    else
        m_ss << "\033[0m";

    m_ss << str << '\n';

    m_instance.write(m_ss.str());
    m_ss.str(std::string());
    m_ss.clear();
}

Logger::Error::Error(Logger& instance) : Message(instance, "ERR") {}
Logger::Debug::Debug(Logger& instance) : Message(instance, "DBG") {}

Logger::Message::Message(const Logger::Message & src) :
    Message(src.m_instance, src.m_level)
{
    m_identCount = src.m_identCount;
}

Logger::Error::Error(const Logger::Error & src):
    Message(src)
{}

Logger::Debug::Debug(const Logger::Debug & src):
    Message(src)
{}
