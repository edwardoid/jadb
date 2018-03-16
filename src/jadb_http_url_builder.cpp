#include "jadb_http_url_builder.h"
#include <sstream>
#include <regex>

using namespace jadb;


UrlBuilder::UrlBuilder(const UrlBuilder& src)
{
    *this = src;
}

const UrlBuilder& UrlBuilder::operator=(const UrlBuilder& src)
{
    m_componenets = src.m_componenets;
    return *this;
}

UrlBuilder& UrlBuilder::number()
{
    m_componenets.push_back("([\\d]+)");
    return *this;
}

UrlBuilder& UrlBuilder::word(std::string word)
{
    m_componenets.push_back(word);
    return *this;
}

UrlBuilder& UrlBuilder::string()
{
    m_componenets.push_back("([\\w]+)");
    return *this;
}

std::string UrlBuilder::url()
{
    std::stringstream ss;
    ss << '^';
    for (auto& token : m_componenets)
    {
        ss << '/' << token;
    }
    ss << '$';
    return ss.str();
}

std::vector<std::string> UrlBuilder::parse(std::string& url) const
{
    std::vector<std::string> tokens;
    size_t offset = 1;
    for (auto component : m_componenets)
    {
        component = "^" + component;
        std::regex rx(component, std::regex_constants::icase);
        std::cmatch match;
        if (!std::regex_search(url.c_str() + offset, match, rx))
        {
            tokens.clear();
            break;
        }
        offset += match.begin()->str().size() + 1;
        tokens.push_back(match.begin()->str());
    }

    return tokens;
}