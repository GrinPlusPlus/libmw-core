#pragma once

#include <unordered_map>
#include <string>

class Config
{
public:
    Config(std::unordered_map<std::string, std::string>&& options)
        : m_options(std::move(options)) { }

    std::string Get(const std::string& key, const std::string& default = "") const noexcept
    {
        auto iter = m_options.find(key);
        return iter != m_options.end() ? iter->second : default;
    }

    void Set(const std::string& key, const std::string& value) noexcept { m_options[key] = value; }

private:
    std::unordered_map<std::string, std::string> m_options;
};