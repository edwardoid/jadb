#include "jadb_http_server.h"
#include "jadb_record.h"
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

    m_createDatabaseUrl.word("database").string();
    m_srv.resource[m_createDatabaseUrl.url()]["PUT"] = [&](std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request) {
        m_api.createDatabase(m_createDatabaseUrl, response, request);
    };

    m_getCollectionsUrl.string();
    m_srv.resource[m_getCollectionsUrl.url()]["GET"] = [&](std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request) {
        m_api.getCollections(m_getCollectionsUrl, response, request);
    };

    m_createCollectionUrl.word("collection").string().string();
    m_srv.resource[m_createCollectionUrl.url()]["PUT"] = [&](std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request) {
        m_api.createCollection(m_createCollectionUrl, response, request);
    };

    m_getRecordUrl.string().string().number();
    m_srv.resource[m_getRecordUrl.url()]["GET"] = [&](std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request) {
        m_api.getRecord(m_getRecordUrl, response, request);
    };

    m_insertRecordUrl.word("records").string().string();
    m_srv.resource[m_insertRecordUrl.url()]["PUT"] = [&](std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request) {
        m_api.insertRecord(m_insertRecordUrl, response, request);
    };

    m_deleteRecordUrl.word("records").string().string().number();
    m_srv.resource[m_deleteRecordUrl.url()]["DELETE"] = [&](std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request) {
        m_api.deleteRecord(m_deleteRecordUrl, response, request);
    };

    m_indexUrl.string().string().word("index").string();
    m_srv.resource[m_indexUrl.url()]["PUT"] = [&](std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request) {
        m_api.createIndex(m_indexUrl, response, request);
    };

    m_searchByIndex.string().string().word("index").word("search").string();
    m_srv.resource[m_searchByIndex.url()]["GET"] = [&](std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request) {
        m_api.searchByIndex(m_searchByIndex, response, request);
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