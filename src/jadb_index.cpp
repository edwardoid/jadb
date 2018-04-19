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

uint32_t Index::operator() (const rapidjson::Value& tree) const
{
    std::ostringstream ss;
    static std::string Empty;
    for (auto& f : m_fields)
    {
        if (!tree.HasMember(f.c_str()))
        {
            return CantCreateIndex;
        }
        ss << tree.FindMember(f.c_str())->value.GetString() << '+';
    }

    auto str = ss.str();
    return static_cast<uint32_t>(hash(reinterpret_cast<const unsigned char*>(str.c_str()), str.size()));
}

const std::vector<std::string>& Index::fields() const
{
    return m_fields;
}