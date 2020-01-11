#pragma once

#include <mw/core/traits/Jsonable.h>
#include <mw/core/exceptions/NetworkException.h>
#include <tl/optional.hpp>
#include <atomic>

namespace RPC
{
    static std::atomic_int ID_COUNTER = 1;

    class Id : public Traits::IJsonable
    {
    public:
        enum class value_t
        {
            null,
            number,
            string
        };

        static Id Create(const int intId) { return Id(value_t::number, intId, ""); }
        static Id Create(const std::string& strId) { return Id(value_t::string, 0, strId); }
        static Id CreateNull() { return Id(value_t::null, 0, ""); }
        static Id Generate() { return Id(value_t::number, ID_COUNTER++, ""); }

        value_t GetType() const noexcept { return m_type; }
        int GetInt() const noexcept { return m_intId; }
        const std::string& GetString() const noexcept { return m_strId; }

        static Id FromJSON(const json& parent)
        {
            Json parser(parent);

            auto iter = parent.find("id");
            if (iter == parent.end() || iter->is_null())
            {
                return CreateNull();
            }
            else if (iter->is_number())
            {
                return Create(iter->get<int>());
            }
            else if (iter->is_string())
            {
                return Create(iter->get<std::string>());
            }

            ThrowNetwork("Failed to parse Id");
        }

        virtual json ToJSON() const override final
        {
            if (m_type == value_t::null)
            {
                return json();
            }
            else if (m_type == value_t::number)
            {
                return json(m_intId);
            }
            else
            {
                return json(m_strId);
            }
        }

    private:
        Id(value_t type, const int intId, const std::string& strId) : m_type(type), m_intId(intId), m_strId(strId) { }

        value_t m_type;
        int m_intId;
        std::string m_strId;
    };
}