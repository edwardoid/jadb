#include "jadb_rest_endpoints.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace jadb;

RESTApi::RESTApi(std::unordered_map<std::string, std::shared_ptr<Database>>& availableDatabases)
    : m_databases(availableDatabases)
{

}

void RESTApi::getDatabasesList(std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request)
{
    boost::property_tree::ptree dbs;

    boost::property_tree::ptree list;
    for (auto db : m_databases)
    {
        boost::property_tree::ptree c;
        c.put("", db.first);
        list.push_back(std::make_pair("", c));
    }
    dbs.add_child("databases", list);

    std::stringstream ss;
    boost::property_tree::write_json(ss, dbs, true);
    response->write(ss);
}

void RESTApi::getCollections(UrlBuilder& url, std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request)
{
    auto urlRaw = request->path_match[0].str();
    auto tokens = url.parse(urlRaw);
    auto name = tokens[0];

    auto dbPos = m_databases.find(name);
    if (dbPos == m_databases.cend())
    {
        response->write(SimpleWeb::StatusCode::client_error_not_found);
        return;
    }

    auto db = dbPos->second;

    boost::property_tree::ptree collections;

    boost::property_tree::ptree list;
    for (auto collection : db->collections())
    {
        boost::property_tree::ptree c;
        c.put("", collection.first);
        list.push_back(std::make_pair("", c));
    }
    collections.add_child("collections", list);

    std::stringstream ss;
    boost::property_tree::write_json(ss, collections, true);
    response->write(ss);
}

void RESTApi::getRecord(UrlBuilder& url, std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request)
{
    auto urlRaw = request->path_match[0].str();
    auto tokens = url.parse(urlRaw);

    auto name = tokens[0];
    auto collection = tokens[1];
    auto id = atoll(tokens[2].c_str());

    auto dbPos = m_databases.find(name);
    if (dbPos == m_databases.cend())
    {
        response->write(SimpleWeb::StatusCode::client_error_not_found);
        return;
    }

    auto db = dbPos->second;

    auto colPos = db->collections().find(collection);
    if (colPos == db->collections().cend())
    {
        response->write(SimpleWeb::StatusCode::client_error_not_found);
        return;
    }

    auto col = colPos->second;

    if (!col->contains(id))
    {
        response->write(SimpleWeb::StatusCode::client_error_not_found);
        return;
    }

    auto rec = col->get(id);

    std::stringstream ss;
    rec.view(ss);
    response->write(ss);
}

void RESTApi::insertRecord(UrlBuilder& url, std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request)
{
    auto urlRaw = request->path_match[0].str();
    auto tokens = url.parse(urlRaw);

    auto name = tokens[0];
    auto collection = tokens[1];

    auto dbPos = m_databases.find(name);
    if (dbPos == m_databases.cend())
    {
        response->write(SimpleWeb::StatusCode::client_error_not_found);
        return;
    }

    auto db = dbPos->second;

    auto colPos = db->collections().find(collection);
    if (colPos == db->collections().cend())
    {
        response->write(SimpleWeb::StatusCode::client_error_not_found);
        return;
    }

    auto col = colPos->second;

    Record rec(request->content.string());

    if (col->insert(rec) == 0)
    {
        response->write(SimpleWeb::StatusCode::client_error_bad_request);
        return;
    }

    std::stringstream ss;
    rec.view(ss);
    response->write(ss);
}

void RESTApi::deleteRecord(UrlBuilder& url, std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request)
{
    auto urlRaw = request->path_match[0].str();
    auto tokens = url.parse(urlRaw);

    auto name = tokens[0];
    auto collection = tokens[1];
    auto id = atoll(tokens[2].c_str());

    auto dbPos = m_databases.find(name);
    if (dbPos == m_databases.cend())
    {
        response->write(SimpleWeb::StatusCode::client_error_not_found);
        return;
    }

    auto db = dbPos->second;

    auto colPos = db->collections().find(collection);
    if (colPos == db->collections().cend())
    {
        response->write(SimpleWeb::StatusCode::client_error_not_found);
        return;
    }

    auto col = colPos->second;

    if (!col->contains(id))
    {
        response->write(SimpleWeb::StatusCode::client_error_not_found);
        return;
    }

    auto rec = col->get(id);
    col->remove(id);
    
    std::stringstream ss;
    rec.view(ss);
    response->write(ss);
}