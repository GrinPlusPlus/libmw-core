#pragma once

#include <mw/core/traits/Printable.h>
#include <nlohmann/json.hpp>
#include <tl/optional.hpp>
#include <type_traits>

using json = nlohmann::json;

class Json : public Traits::IPrintable
{
public:
    Json(const json& json) : m_json(json) { }
    virtual ~Json() = default;

    template<class T>
    tl::optional<typename std::enable_if_t<std::is_arithmetic_v<T>, T>> Get(const std::string& key) const
    {
        auto iter = m_json.find(key);
        if (iter != m_json.end())
        {
            if (iter->is_number())
            {
                return tl::make_optional<T>(iter->get<T>());
            }
        }

        return tl::nullopt;
    }

    template<class T>
    tl::optional<typename std::enable_if_t<std::is_same_v<bool, T>, T>> Get(const std::string& key) const
    {
        auto iter = m_json.find(key);
        if (iter != m_json.end())
        {
            if (iter->is_bool())
            {
                return tl::make_optional<T>(iter->get<T>());
            }
        }

        return tl::nullopt;
    }

    template<class T>
    tl::optional<typename std::enable_if_t<std::is_base_of_v<T, std::string>, T>> Get(const std::string& key) const
    {
        auto iter = m_json.find(key);
        if (iter != m_json.end())
        {
            if (iter->is_string())
            {
                return tl::make_optional<T>(iter->get<T>());
            }
        }

        return tl::nullopt;
    }

    template<class T>
    tl::optional<typename std::enable_if_t<std::is_same_v<T, json>, T>> Get(const std::string& key) const
    {
        auto iter = m_json.find(key);
        if (iter != m_json.end())
        {
            if (iter->is_object() || iter->is_array())
            {
                return tl::make_optional<T>(*iter);
            }
        }

        return tl::nullopt;
    }

    virtual std::string Format() const override final { return m_json.dump(); }

private:
    const json& m_json;
};

/*template<class T>
json ToJSON(const std::vector<T>& elements)
{
    std::vector<json> jsonElems;
    for (const T& elem)
    {
        jsonElems.push_back(elem.ToJSON());
    }
}*/