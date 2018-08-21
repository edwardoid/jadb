#include "jadb_search_options.h"
#include <limits>

using namespace jadb;


SearchOptions::SearchOptions(const nlohmann::json& options)
    : m_limit(std::numeric_limits<size_t>::max())
    , m_offset(0), m_saved(false)
{
}