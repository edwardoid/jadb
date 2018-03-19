#include "jadb_index.h"
#include "jadb_utils.h"

using namespace jadb;

const uint32_t Index::CantCreateIndex = uint32_t(-1);

Index::Index(std::vector<std::string> fields)
    : m_fields(fields)
{}

uint32_t Index::operator() (const Record& rec) const
{
    std::ostringstream ss;
    for (auto& f : m_fields)
    {
        auto v = rec[f];
        if (v.empty())
            return CantCreateIndex;
        ss << v << '+';
    }

    auto str = ss.str();
    return hash(reinterpret_cast<const unsigned char*>(str.c_str()), static_cast<uint32_t>(str.size()));
}

uint32_t Index::operator() (const boost::property_tree::ptree& tree) const
{
    std::ostringstream ss;
    static std::string Empty;
    for (auto& f : m_fields)
    {
        auto v = tree.get<std::string>(f, Empty);
        if (v.empty())
            return CantCreateIndex;
        ss << v << '+';
    }

    auto str = ss.str();
    return hash(reinterpret_cast<const unsigned char*>(str.c_str()), str.size());
}

const std::vector<std::string>& Index::fields() const
{
    return m_fields;
}