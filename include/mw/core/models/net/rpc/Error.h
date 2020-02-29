#pragma once

#include <mw/core/traits/Jsonable.h>
#include <mw/core/exceptions/NetworkException.h>
#include <tl/optional.hpp>

namespace RPC
{
    enum ErrorCode
    {
        INVALID_JSON = -32700,          // Parse error
        INVALID_REQUEST = -32600,       // Invalid Request
        METHOD_NOT_FOUND = -32601,      // Method not found
        INVALID_PARAMS = -32602,        // Invalid params
        INTERNAL_ERROR = -32603         // Internal error
    };

    class Error : public Traits::IJsonable
    {
    public:
        Error(const int code, const std::string& message, const tl::optional<json>& data)
            : m_code(code), m_message(message), m_data(data)
        {

        }

        int GetCode() const noexcept { return m_code; }
        const std::string& GetMsg() const noexcept { return m_message; }
        const tl::optional<json>& GetData() const noexcept { return m_data; }

        static Error FromJSON(const Json& error)
        {
            auto codeOpt = error.Get<int>("code");
            auto messageOpt = error.Get<std::string>("message");
            tl::optional<json> dataOpt = error.Get<json>("data");
            if (!codeOpt.has_value() || !messageOpt.has_value())
            {
                ThrowNetwork_F("Invalid json: {}", error);
            }

            return Error(codeOpt.value(), messageOpt.value(), dataOpt);
        }

        json ToJSON() const noexcept final
        {
            json json({
                {"code", m_code},
                {"message", m_message}
            });

            if (m_data.has_value())
            {
                json["data"] = m_data.value();
            }

            return json;
        }

    private:
        int m_code;
        std::string m_message;
        tl::optional<json> m_data;
    };
}