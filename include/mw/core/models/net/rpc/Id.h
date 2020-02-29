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

        static Id FromJSON(const Json& parent)
        {
            auto idOpt = parent.Get<json>("id");
            if (!idOpt.has_value() || idOpt.value().is_null())
            {
                return CreateNull();
            }
            else if (idOpt.value().is_number())
            {
                return Create(idOpt.value().get<int>());
            }
            else if (idOpt.value().is_string())
            {
                return Create(idOpt.value().get<std::string>());
            }

            ThrowNetwork("Failed to parse Id");
        }

        json ToJSON() const noexcept final
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