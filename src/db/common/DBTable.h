#pragma once

#include "DBEntry.h"

#include <leveldb/db.h>
#include <cassert>
#include <memory>
#include <string>

class DBTable
{
public:
    struct Options
    {
        static Options Default() { return Options(); }
    };

    DBTable(const std::string& prefix, const Options& options = Options::Default())
        : m_prefix(prefix), m_options(options)
    {
        assert(!m_prefix.empty() && prefix.find('_') == std::string::npos);
    }

    std::string BuildKey(const std::string& itemKey) const noexcept { return m_prefix + "_" + itemKey; }

    template<typename T,
        typename SFINAE = typename std::enable_if_t<std::is_base_of_v<Traits::ISerializable, T>>>
    std::string BuildKey(const DBEntry<T>& item) const noexcept { return BuildKey(item.key); }

private:
    std::string m_prefix;
    Options m_options;
};