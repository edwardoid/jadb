#ifndef JADB_HTTP_SERVER_H
#define JADB_HTTP_SERVER_H

#include "sws/server_http.hpp"
#include "jadb.h"
#include "jadb_rest_endpoints.h"
#include "jadb_http_url_builder.h"
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
        RESTApi m_api;
        UrlBuilder m_getCollectionsUrl;
        UrlBuilder m_getRecordUrl;
        UrlBuilder m_insertRecordUrl;
        UrlBuilder m_deleteRecordUrl;
        UrlBuilder m_indexUrl;
        UrlBuilder m_searchByIndex;
        SimpleWeb::Server<SimpleWeb::HTTP> m_srv;
        std::unordered_map<std::string, std::shared_ptr<Database>> m_databases;
    };
}

#endif // JADB_HTTP_SERVER_H