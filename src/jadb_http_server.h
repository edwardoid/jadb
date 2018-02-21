#ifndef JADB_HTTP_SERVER_H
#define JADB_HTTP_SERVER_H

#include "sws/server_http.hpp"
#include "jadb.h"
#include <string>
#include <memory>
#include <unordered_map>

namespace jadb
{
	class HttpServer
	{
	public:
		HttpServer(int port, std::unordered_map<std::string, std::shared_ptr<Database>>& availableDatabases);
		~HttpServer();
		void start();
		void stop();
	private:
		void setupEndpoints();
	private:
		SimpleWeb::Server<SimpleWeb::HTTP> m_srv;
		std::unordered_map<std::string, std::shared_ptr<Database>> m_databases;
	};
}

#endif // JADB_HTTP_SERVER_H