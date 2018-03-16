#ifndef JADB_REST_ENDPOINTS_H
#define JADB_REST_ENDPOINTS_H

#include "jadb.h"
#include "jadb_record.h"
#include "jadb_http_url_builder.h"
#include <memory>
#include "sws/server_http.hpp"

using HttpServerImpl = SimpleWeb::Server<SimpleWeb::HTTP>;

namespace jadb
{
    class RESTApi
    {
    public:
        RESTApi(std::unordered_map<std::string, std::shared_ptr<Database>>& availableDatabases);
        void getDatabasesList(std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request);
        void getCollections(UrlBuilder& url, std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request);
        void getRecord(UrlBuilder& url, std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request);
        void insertRecord(UrlBuilder& url, std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request);
        void deleteRecord(UrlBuilder& url, std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request);
    private:
        std::unordered_map<std::string, std::shared_ptr<Database>> m_databases;
    };
}

#endif // JADB_REST_ENDPOINTS_H