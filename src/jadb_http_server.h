#ifndef JADB_HTTP_SERVER_H
#define JADB_HTTP_SERVER_H

#include "sws/server_http.hpp"
#include "jadb_database.h"
#include "jadb_rest_endpoints.h"
#include "jadb_http_url_builder.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <algorithm>

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

        UrlBuilder m_createDatabaseUrl;     // PUT      /<name>
        UrlBuilder m_getCollectionsUrl;     // GET      /<db name>
        UrlBuilder m_createCollectionUrl;   // PUT      /<db name>/<collection name>
        UrlBuilder m_getRecordUrl;          // GET      /<db name>/<collection name>/<id>
        UrlBuilder m_insertRecordUrl;       // PATCH    /<db name>/<collection name>
        UrlBuilder m_deleteRecordUrl;       // DELETE   /<db name>/<collection name>/<id>
        UrlBuilder m_indexUrl;              // PUT      /<db name>/<collection name>/index/<name>
        UrlBuilder m_searchByIndex;         // GET      /<db name>/<collection name>/index/search/<name>
        UrlBuilder m_query;                 // OPTIONS  /<db name>/<collection name>/query
        SimpleWeb::Server<SimpleWeb::HTTP> m_srv;
        std::unordered_map<std::string, std::shared_ptr<Database>> m_databases;
    };
}

#endif // JADB_HTTP_SERVER_H