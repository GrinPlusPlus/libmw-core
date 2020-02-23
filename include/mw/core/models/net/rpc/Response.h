#pragma once

#include <mw/core/models/net/rpc/Id.h>
#include <mw/core/models/net/rpc/Error.h>
#include <mw/core/traits/Jsonable.h>
#include <mw/core/exceptions/NetworkException.h>
#include <tl/optional.hpp>

namespace RPC
{
    class Response : public Traits::IJsonable
    {
    public:
        Response(const Response& other) = default;

        static Response BuildResult(const Id& id, const json& result)
        {
            return Response(id, tl::make_optional(result), tl::nullopt);
        }

        static Response BuildError(
            const Id& id,
            const int code,
            const std::string& message,
            const tl::optional<json>& data = tl::nullopt)
        {
            return Response(id, tl::nullopt, tl::make_optional(Error(code, message, data)));
        }

        static Response FromJSON(const Json& response)
        {
            // Parse id
            Id id = Id::FromJSON(response);

            // Parse jsonrpc
            tl::optional<std::string> jsonrpcOpt = response.Get<std::string>("jsonrpc");
            if (!jsonrpcOpt.has_value())
            {
                ThrowNetwork("jsonrpc is missing");
            }
            else if (jsonrpcOpt.value() != "2.0")
            {
                ThrowNetwork_F("invalid jsonrpc value: {}", jsonrpcOpt.value());
            }

            // Parse result/error
            tl::optional<json> errorOpt = response.Get<json>("error");
            if (errorOpt.has_value())
            {
                return Response(id, tl::nullopt, tl::make_optional(Error::FromJSON(errorOpt.value())));
            }
            else
            {
                return Response(id, response.Get<json>("result"), tl::nullopt);
            }
        }

        virtual json ToJSON() const noexcept override final
        {
            json json;
            json["jsonrpc"] = "2.0";
            json["id"] = m_id.ToJSON();

            if (m_resultOpt.has_value())
            {
                json["result"] = m_resultOpt.value();
            }
            else
            {
                json["error"] = m_errorOpt.value().ToJSON();
            }

            return json;
        }

        const Id& GetId() const noexcept { return m_id; }

        bool HasResult() const noexcept { return m_resultOpt.has_value(); }
        const tl::optional<json>& GetResult() const noexcept { return m_resultOpt; }

        bool HasError() const noexcept { return m_errorOpt.has_value(); }
        const tl::optional<Error>& GetError() const noexcept { return m_errorOpt; }

    private:
        Response(const Id& id, tl::optional<json>&& resultOpt, tl::optional<Error>&& errorOpt)
            : m_id(id), m_resultOpt(std::move(resultOpt)), m_errorOpt(std::move(errorOpt))
        {

        }

        Id m_id;
        tl::optional<json> m_resultOpt;
        tl::optional<Error> m_errorOpt;
    };
}