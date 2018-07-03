#include "jadb_index.h"
#include "jadb_utils.h"

using namespace jadb;

const uint32_t Index::CantCreateIndex = uint32_t(-1);

Index::Index(std::vector<std::string> fields)
    : m_fields(fields)
{}

uint32_t Index::operator() (const Record& rec) const
{
    return (*this)(rec.data());
}

uint32_t Index::operator() (const nlohmann::json& tree) const
{
    std::ostringstream ss;
    static std::string Empty;
    for (auto& f : m_fields)
    {
        if (tree.find(f) == tree.cend())
        {
            return CantCreateIndex;
        }
        ss << tree[f] << '+';
    }

    auto str = ss.str();
    return static_cast<uint32_t>(hash(reinterpret_cast<const unsigned char*>(str.c_str()), str.size()));
}

const std::vector<std::string>& Index::fields() const
{
    return m_fields;
}