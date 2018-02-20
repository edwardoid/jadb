#include "jadb_logger.h"
#include <chrono>
#include <ctime>

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

Logger::Message Logger::msg()
{
	return Message(log());
}

Logger::Error Logger::err()
{
	return Error(log());
}

Logger::Debug Logger::debug()
{
	return Debug(log());
}

Logger::Message::Message(Logger& instance, std::string level)
	: m_instance(instance), m_level(level)
{}

Logger::Message& Logger::Message:: operator << (const int& val) { m_ss << val; return *this; }
Logger::Message& Logger::Message:: operator << (const double& val) { m_ss << val; return *this; }
Logger::Message& Logger::Message:: operator << (const std::string& msg) { m_ss << msg; return *this; }

void Logger::Message::end()
{
	auto str = m_ss.str();
	if (str.empty())
		return;

	m_ss.str(std::string());
	m_ss.clear();
	auto now = std::chrono::system_clock::now();
	time_t ctime_now = (time_t)(std::chrono::system_clock::to_time_t(now));
	m_ss << std::ctime(&ctime_now) << " ["<< m_level << "] : " << str << '\n';

	m_instance.write(m_ss.str());
	m_ss.str(std::string());
	m_ss.clear();
}

Logger::Error::Error(Logger& instance) : Message(instance, "ERR") {}
Logger::Debug::Debug(Logger& instance) : Message(instance, "DBG") {}

Logger::Message::Message(const Logger::Message & src) :
	Message(src.m_instance, src.m_level)
{}

Logger::Error::Error(const Logger::Error & src):
	Message(src)
{}

Logger::Debug::Debug(const Logger::Debug & src):
	Message(src)
{}