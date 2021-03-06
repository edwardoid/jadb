#include "jadb_http_server.h"
#include "jadb_logger.h"
#include "jadb_database.h"
#include "jadb_configuration.h"

#include "jadb_id_mapping.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/program_options.hpp>
#include <assert.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <csignal>

#include <nlohmann/json.hpp>

void help()
{
    jadb::Logger::msg() << "Usage: jadb --config=<cofig file path>";
}

void start(boost::filesystem::path root)
{   
    jadb::Configuration::load(root);

    std::unordered_map<std::string, std::shared_ptr<jadb::Database>> databases;


    boost::filesystem::create_directories(jadb::Configuration::root());
    boost::filesystem::directory_iterator it(jadb::Configuration::root()), end;
    {
        jadb::Logger::msg() << "Looking for databases in " << jadb::Configuration::root().generic_string();
    }
    for (; it != end; ++it)
    {
        boost::filesystem::path path = it->path();
        if (boost::filesystem::is_directory(path))
        {
            auto name = path.filename().generic_string();
            jadb::Logger::msg() << "Found database " << name;
            databases.insert(std::make_pair(name, std::make_shared<jadb::Database>(name)));
        }
    }

    jadb::HttpServer srv(jadb::Configuration::port(), databases);

    srv.start();
}


void stop(int sig)
{
    {
        jadb::Logger::msg() << "Shutting down...";

    }
    jadb::Configuration::save();
    std::exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)
{
    boost::program_options::options_description ops;


#ifdef __linux__
    boost::filesystem::path root = ".";
#else
    boost::filesystem::path root = "D:\\db";
#endif // __linux
    root.append("config.json");

    ops.add_options()
        ("help,h", "Get help")
        ("config", boost::program_options::value<boost::filesystem::path>(&root), "Specify configuration file")
    ;


    std::signal(SIGTERM, stop);
    std::signal(SIGSEGV, stop);
    std::signal(SIGINT, stop);
    std::signal(SIGABRT, stop);
    std::signal(SIGFPE, stop);
    std::signal(SO_BSDCOMPAT, stop);

    boost::program_options::variables_map arguments;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, ops), arguments);

    if (arguments.count("help") || arguments.count("h"))
    {
        help();
        return 0;
    }

    root = arguments.at("config").as<boost::filesystem::path>();
    start(root);
    return EXIT_SUCCESS;
}
