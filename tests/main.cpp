#include <jadb_logger.h>
#include <jadb_data_file.h>
#include <jadb.h>
#include <boost\property_tree\ptree.hpp>

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
	jadb::Database db("D:\\db");
	jadb::Record entry(JSON_DATA);
	db.create("users").insert(entry);
	db.create("users").get(1);
}

int main(int argc, char** argv)
{
	test();
	return EXIT_SUCCESS;
}