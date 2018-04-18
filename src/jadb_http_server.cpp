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
    // GET /
    m_srv.resource["^/$"]["GET"] = [&](std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request) {
        m_api.getDatabasesList(response, request);
    };

    // PUT /<name>
    m_createDatabaseUrl.string();
    m_srv.resource[m_createDatabaseUrl.url()]["PUT"] = [&](std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request) {
        m_api.createDatabase(m_createDatabaseUrl, response, request);
    };

    // GET /<name>
    m_getCollectionsUrl.string();
    m_srv.resource[m_getCollectionsUrl.url()]["GET"] = [&](std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request) {
        m_api.getCollections(m_getCollectionsUrl, response, request);
    };

    // PUT /<db name>/<collection name>
    m_createCollectionUrl.string().string();
    m_srv.resource[m_createCollectionUrl.url()]["PUT"] = [&](std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request) {
        m_api.createCollection(m_createCollectionUrl, response, request);
    };
    
    // GET /<db name>/<collection name>/<id>
    m_getRecordUrl.string().string().number();
    m_srv.resource[m_getRecordUrl.url()]["GET"] = [&](std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request) {
        m_api.getRecord(m_getRecordUrl, response, request);
    };

    // PATCH /<db name>/<collection name>
    m_insertRecordUrl.string().string();
    m_srv.resource[m_insertRecordUrl.url()]["PATCH"] = [&](std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request) {
        m_api.insertRecord(m_insertRecordUrl, response, request);
    };

    // DELETE /<db name>/<collection name>/<id>
    m_deleteRecordUrl.string().string().number();
    m_srv.resource[m_deleteRecordUrl.url()]["DELETE"] = [&](std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request) {
        m_api.deleteRecord(m_deleteRecordUrl, response, request);
    };

    // PUT /<db name>/<collection name>/index/<name>
    m_indexUrl.string().string().word("index").string();
    m_srv.resource[m_indexUrl.url()]["PUT"] = [&](std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request) {
        m_api.createIndex(m_indexUrl, response, request);
    };

    // GET /<db name>/<collection name>/index/search/<name>
    m_searchByIndex.string().string().word("index").word("search").string();
    m_srv.resource[m_searchByIndex.url()]["GET"] = [&](std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request) {
        m_api.searchByIndex(m_searchByIndex, response, request);
    };

    // OPTIONS /<db name>/<collection name>/query
    m_query.string().string().word("query");
    m_srv.resource[m_query.url()]["OPTIONS"] = [&](std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request) {
        m_api.query(m_query, response, request);
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