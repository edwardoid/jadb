#include "jadb_rest_endpoints.h"
#include <nlohmann/json.hpp>
#include "jadb_query.h"

using namespace jadb;

void WriteJson(std::shared_ptr<HttpServerImpl::Response> response, nlohmann::json& doc)
{
    std::stringstream ss;
    ss << doc.dump(4);
    response->write(ss);
}

RESTApi::RESTApi(std::unordered_map<std::string, std::shared_ptr<Database>>& availableDatabases)
    : m_databases(availableDatabases)
{}

void RESTApi::getDatabasesList(std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request)
{
    nlohmann::json list = nlohmann::json::array();
    for (auto db : m_databases)
    {
        list.push_back(db.first);
    }
    nlohmann::json res = { { "databases", list } };
    WriteJson(response, res);
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

    nlohmann::json list = nlohmann::json::array();;
    for (auto collection : db->collections())
    {
        list.push_back(collection.first);
    }

    nlohmann::json collections = { { "collections", list } };
    WriteJson(response, collections);
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

    WriteJson(response, rec.m_data);
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

    WriteJson(response, rec.m_data);
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

    nlohmann::json fields;

    try
    {
        fields = nlohmann::json::parse(request->content.string());
    }
    catch(...)
    {
        response->write(SimpleWeb::StatusCode::client_error_bad_request);
        return;
    }

    if (fields.empty() == 0)
    {
        response->write(SimpleWeb::StatusCode::client_error_bad_request);
        return;
    }

    auto fieldsIt = fields.find("fields");    
    if (fieldsIt == fields.end() || !fieldsIt->is_array())
    {
        response->write(SimpleWeb::StatusCode::client_error_bad_request);
        return;
    }

    std::vector<std::string> names;
    auto& fieldsMember = *fieldsIt;
    for (auto i = 0; i != fieldsMember.size(); ++i)
    {
        if (!fieldsMember[i].is_string())
        {
            response->write(SimpleWeb::StatusCode::client_error_bad_request);
            return;
        }
        names.push_back((nlohmann::json::string_t)fieldsMember[i]);
    }
    std::sort(names.begin(), names.end());
    auto col = colPos->second;
    col->createIndex(name, names);

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

    nlohmann::json list = nlohmann::json::array();
    for (auto& r : res)
    {
        list.push_back(r.data());
    }

    nlohmann::json doc = {
        { "items" , list},
        { "items" , res.size() }
    };

    WriteJson(response, doc);
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

    try
    {
        nlohmann::json doc = nlohmann::json::parse(request->content.string());
        Query q;
        if (!q.create(doc))
        {
            response->write(SimpleWeb::StatusCode::client_error_bad_request);
            return;
        }
        auto collection = database->second->collections()[name];
        auto res = collection->query(q);

        nlohmann::json list = nlohmann::json::array();
        for (auto& r : res)
        {
            list.push_back(r.data());
        }

        nlohmann::json resp = {
            { "docs" , list},
            { "count" , res.size() }
        };

        WriteJson(response, resp);
    }
    catch(...)
    {
        response->write(SimpleWeb::StatusCode::client_error_bad_request);
        return;
    }
}
