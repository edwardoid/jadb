#include "jadb_rest_endpoints.h"
#include "jadb_query.h"
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/reader.h>

using namespace jadb;

void WriteJson(std::shared_ptr<HttpServerImpl::Response> response, rapidjson::Document& doc)
{
    rapidjson::StringBuffer buff;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buff);
    doc.Accept(writer);

    std::stringstream ss;
    ss << buff.GetString();
    response->write(ss);
}

RESTApi::RESTApi(std::unordered_map<std::string, std::shared_ptr<Database>>& availableDatabases)
    : m_databases(availableDatabases)
{}

void RESTApi::getDatabasesList(std::shared_ptr<HttpServerImpl::Response> response, std::shared_ptr<HttpServerImpl::Request> request)
{
    rapidjson::Document res(rapidjson::kObjectType);
    rapidjson::Value list(rapidjson::kArrayType);

    for (auto db : m_databases)
    {
        rapidjson::Value name(db.first.c_str(), res.GetAllocator());
        list.PushBack(name, res.GetAllocator());
    }
    res.AddMember("databases", list, res.GetAllocator());
    
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

    rapidjson::Document collections(rapidjson::kObjectType);

    rapidjson::Value list(rapidjson::kArrayType);
    for (auto collection : db->collections())
    {
        rapidjson::Value col(collection.first.c_str(), collections.GetAllocator());
        list.PushBack(col, collections.GetAllocator());
    }
    collections.AddMember("collections", list, collections.GetAllocator());

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

    rapidjson::IStreamWrapper is(request->content);
    rapidjson::Document fields;

    fields.ParseStream(is);

    if (fields.MemberCount() == 0 || fields.HasParseError())
    {
        response->write(SimpleWeb::StatusCode::client_error_bad_request);
        return;
    }

    if (!fields.HasMember("fields"))
    {
        response->write(SimpleWeb::StatusCode::client_error_bad_request);
        return;
    }

    auto fieldsMember = fields.FindMember("fields");
    if (!fieldsMember->value.IsArray())
    {
        response->write(SimpleWeb::StatusCode::client_error_bad_request);
        return;
    }

    auto fieldsList = fieldsMember->value.GetArray();
    std::vector<std::string> names;
    for (auto i = 0; i != fieldsList.Size(); ++i)
    {
        if (!fieldsList[i].IsString())
        {
            response->write(SimpleWeb::StatusCode::client_error_bad_request);
            return;
        }
        auto f = fieldsList[i].GetString();
        names.push_back(f);
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
    
    rapidjson::Document doc(rapidjson::kObjectType);

    rapidjson::Value list(rapidjson::kArrayType);
    for (auto& r : res)
    {
        rapidjson::Value val;
        val.CopyFrom(r.data(), doc.GetAllocator());
        list.PushBack(val, doc.GetAllocator());
    }
    doc.AddMember("items", list, doc.GetAllocator());
    doc.AddMember("count", static_cast<uint32_t>(res.size()), doc.GetAllocator());

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
