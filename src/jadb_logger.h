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
            Message& operator << (const uint32_t& val);
            Message& operator << (const size_t& val);
            Message& operator << (const double& val);
            Message& operator << (const std::string& msg);
            ~Message() { end(); }
            void end();
        protected:
            void ident(uint8_t count);
            friend class Logger;
        protected:
            size_t m_identCount = 0;
            Logger& m_instance;
            std::string m_level;
            std::stringstream m_ss;
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

        static Message msg(uint8_t ident = 0);
        static Error err(uint8_t ident = 0);
        static Debug dbg(uint8_t ident = 0);

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
