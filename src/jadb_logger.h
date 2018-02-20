#ifndef JADB_LOGGER_H
#define JADB_LOGGER_H

#include <iostream>
#include <sstream>

namespace jadb
{
	class Logger
	{
	public:

		class Message
		{
		public:
			Message(Logger& src, std::string level = "MSG");
			Message(const Message& src);
			Message& operator << (const int& val);
			Message& operator << (const double& val);
			Message& operator << (const std::string& msg);
			~Message() { end(); }
			void end();
		protected:
			std::string m_level;
			std::stringstream m_ss;
			Logger& m_instance;
		};

		class Error : public Message
		{
		public:
			Error(Logger& src);
			Error(const Error& src);
		};

		class Debug : public Message
		{
		public:
			Debug(Logger& src);
			Debug(const Debug& debug);
		};

		static Message msg();
		static Error err();
		static Debug debug();

	protected:
		static Logger& log();
		static void write(const std::string& msg);

	private:
		Logger(const Logger&) = delete;
		Logger() = default;
		const Logger& operator = (const Logger&) = delete;
	};
}

#endif // JADB_LOGGER_H