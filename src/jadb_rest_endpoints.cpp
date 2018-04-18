#include "jadb_rest_endpoints.h"
#include "jadb_query.h"
#include <rapidjson/document.h>
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

void RESTApi::createDatabase(UrlBuilder & url, std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request)
{
    auto urlRaw = request->path_match[0].str();
    auto tokens = url.parse(urlRaw);
    auto name = tokens[0];

    if (m_databases.find(name) != m_databases.end())
    {
        response->write(SimpleWeb::StatusCode::client_error_conflict);
        return;
    }
    
    auto db = std::make_shared<jadb::Database>(name);


    m_databases.insert(std::make_pair(name, db));
    response->write(SimpleWeb::StatusCode::success_created);
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

void RESTApi::createCollection(UrlBuilder & url, std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request)
{
    auto urlRaw = request->path_match[0].str();
    auto tokens = url.parse(urlRaw);
    auto db = tokens[0];
    auto name = tokens[1];

    auto database = m_databases.find(db);
    if (database == m_databases.end())
    {
        response->write(SimpleWeb::StatusCode::client_error_not_found);
        return;
    }
    if(database->second->has(name))
    {
        response->write(SimpleWeb::StatusCode::client_error_conflict);
        return;
    }
    
    database->second->create(name);

    response->write(SimpleWeb::StatusCode::success_created);
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

void RESTApi::createIndex(UrlBuilder& url, std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request)
{
    auto urlRaw = request->path_match[0].str();
    auto tokens = url.parse(urlRaw);

    auto database = tokens[0];
    auto collection = tokens[1];
    auto name = tokens[3];

    auto dbPos = m_databases.find(database);
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

    std::istringstream is(request->content.string());
    try
    {
        boost::property_tree::ptree fields;
        boost::property_tree::read_json(is, fields);
        if (fields.empty())
        {
            response->write(SimpleWeb::StatusCode::client_error_bad_request);
            return;
        }
        auto fieldsPos = fields.find("fields");
        std::vector<std::string> names;
        auto fieldsChild = fields.get_child_optional("fields");
        if (!fieldsChild)
        {
            response->write(SimpleWeb::StatusCode::client_error_bad_request);
            return;
        }

        for (auto i = fieldsChild.get().begin(); i != fieldsChild.get().end(); ++i)
        {
            auto f = i->second.get_value<std::string>("");
            if (f.empty())
                continue;
            names.push_back(f);
        }
        std::sort(names.begin(), names.end());
        auto col = colPos->second;
        col->createIndex(name, names);
    }
    catch (boost::property_tree::json_parser_error e)
    {
        response->write(SimpleWeb::StatusCode::client_error_bad_request);
    }

    response->write(SimpleWeb::StatusCode::success_created);
}

void RESTApi::searchByIndex(UrlBuilder& url, std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request)
{
    auto urlRaw = request->path_match[0].str();
    auto tokens = url.parse(urlRaw);

    auto database = tokens[0];
    auto collection = tokens[1];
    auto name = tokens[4];

    auto dbPos = m_databases.find(database);
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

    if (!colPos->second->hasIndex(name))
    {
        response->write(SimpleWeb::StatusCode::client_error_not_found);
        return;
    }

    auto query = request->parse_query_string();
    size_t skip = 0;
    auto skipPos = query.find("skip");
    if (skipPos != query.end())
    {
        skip = atoll(skipPos->second.c_str());
        query.erase(skipPos);
    }

    size_t limit = 999;
    auto limitPos = query.find("limit");
    if (limitPos != query.end())
    {
        limit = atoll(limitPos->second.c_str());
        query.erase(limitPos);
    }

    std::unordered_map<std::string, std::string> q;
    for (auto& i : query)
    {
        q.insert(std::make_pair(i.first, i.second));
    }

    auto res = colPos->second->searchByIndex(name, q, limit, skip);
    
    boost::property_tree::ptree results;

    boost::property_tree::ptree list;
    for (auto& r : res)
    {
        list.push_back(std::make_pair("", r.data()));
    }
    results.add_child("items", list);
    results.put<int>("count", static_cast<const int>(res.size()));

    std::stringstream ss;
    boost::property_tree::write_json(ss, results, true);
    response->write(ss);
}
void RESTApi::query(UrlBuilder& url, std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request)
{
    auto urlRaw = request->path_match[0].str();
    auto tokens = url.parse(urlRaw);
    auto db = tokens[0];
    auto name = tokens[1];

    auto database = m_databases.find(db);
    if (database == m_databases.end())
    {
        response->write(SimpleWeb::StatusCode::client_error_not_found);
        return;
    }
    if (!database->second->has(name))
    {
        response->write(SimpleWeb::StatusCode::client_error_conflict);
        return;
    }

    rapidjson::Document doc;
    doc.Parse(request->content.string().c_str());
    if (doc.HasParseError())
    {
        response->write(SimpleWeb::StatusCode::client_error_bad_request);
        return;
    }

    Query q;
    if (!q.create(doc))
    {
        response->write(SimpleWeb::StatusCode::client_error_bad_request);
        return;
    }
    auto collection = database->second->collections()[name];
    collection->query(q);
}
