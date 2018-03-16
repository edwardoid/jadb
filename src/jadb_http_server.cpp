#include "jadb_http_server.h"
#include "jadb_record.h"
#include "jadb_http_url_builder.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <vector>
#include <sstream>

using namespace jadb;

using HttpServerImpl = SimpleWeb::Server<SimpleWeb::HTTP>;

HttpServer::HttpServer(int port, std::unordered_map<std::string, std::shared_ptr<Database>>& availableDatabases)
    : m_api(availableDatabases)
{
    m_srv.config.port = port;
    m_srv.config.address = "127.0.0.1";
    m_databases = availableDatabases;
    setupEndpoints();
}

void HttpServer::setupEndpoints()
{
    m_srv.resource["^/$"]["GET"] = [&](std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request) {
        m_api.getDatabasesList(response, request);
    };

    
    UrlBuilder getCollectionsUrl;
    getCollectionsUrl.string();
    m_srv.resource[getCollectionsUrl.url()]["GET"] = [&](std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request) {
        m_api.getCollections(getCollectionsUrl, response, request);
    };

    UrlBuilder getRecordUrl;
    getRecordUrl.string().string().number();
    m_srv.resource[getRecordUrl.url()]["GET"] = [&](std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request) {
        m_api.getRecord(getRecordUrl, response, request);
    };

    UrlBuilder insertRecordUrl;
    insertRecordUrl.string().string();
    m_srv.resource[insertRecordUrl.url()]["PUT"] = [&](std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request) {
        m_api.insertRecord(insertRecordUrl, response, request);
    };

    UrlBuilder deleteRecordUrl;
    deleteRecordUrl.string().string().number();
    m_srv.resource[deleteRecordUrl.url()]["DEL"] = [&](std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request) {
        m_api.deleteRecord(deleteRecordUrl, response, request);
    };
}

HttpServer::~HttpServer()
{
    stop();
}

void HttpServer::start()
{
    m_srv.start();
}

void HttpServer::stop()
{
    m_srv.stop();
}