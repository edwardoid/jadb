#include <jadb_logger.h>
#include <jadb_data_file.h>
#include <jadb.h>
#include <jadb_http_server.h>
#include <boost/property_tree/ptree.hpp>
#include <assert.h>
#include <unordered_map>
#include <string>
#include <memory>

const char* JSON_DATA = R"(
{
	"employees":[
		{"firstName":"John", "lastName" : "Doe"},
		{ "firstName":"Anna", "lastName" : "Smith" },
		{ "firstName":"Peter", "lastName" : "Jones" }
	]
}
)";

void test()
{
	std::unordered_map<std::string, std::shared_ptr<jadb::Database>> databases;
#ifdef __linux__
	auto db = std::make_shared<jadb::Database>("/home/edward/Programming/jadb/tests");
#else
	auto db = std::make_shared<jadb::Database>("D:\\db");
#endif // __linux
	jadb::Record entry(JSON_DATA);
	db->create("users").insert(entry);
	db->create("users").get(1);
	assert(db->create("users").contains(1));
	
	db->create("admins");
	db->create("logs");

	databases.insert(std::make_pair("first", db));

	jadb::HttpServer srv(80, databases);
	srv.start();
}

int main(int argc, char** argv)
{
	test();
	return EXIT_SUCCESS;
}
