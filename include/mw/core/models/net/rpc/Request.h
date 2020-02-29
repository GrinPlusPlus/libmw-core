#pragma once

#include <mw/core/models/net/rpc/Id.h>
#include <mw/core/models/net/rpc/Error.h>
#include <mw/core/models/net/rpc/Response.h>
#include <mw/core/traits/Jsonable.h>
#include <mw/core/exceptions/NetworkException.h>
#include <tl/optional.hpp>

namespace RPC
{
    class Request : public Traits::IJsonable
    {
    public:
        static Request FromJSON(const Json& request)
        {
            // Parse id
            Id id = Id::FromJSON(request);

            // Parse jsonrpc
            tl::optional<std::string> jsonrpcOpt = request.Get<std::string>("jsonrpc");
            if (!jsonrpcOpt.has_value())
            {
                ThrowNetwork("jsonrpc is missing");
            }
            else if (jsonrpcOpt.value() != "2.0")
            {
                ThrowNetwork_F("invalid jsonrpc value: {}", jsonrpcOpt.value());
            }

            // Parse method
            tl::optional<std::string> methodOpt = request.Get<std::string>("method");
            if (!methodOpt.has_value() || methodOpt.value().empty())
            {
                ThrowNetwork("method is missing");
            }

            // Parse params
            tl::optional<json> paramsOpt = request.Get<json>("params");

            return Request(id, methodOpt.value(), std::move(paramsOpt));
        }

        json ToJSON() const noexcept final
        {
            json json({
                {"id", m_id.ToJSON()},
                {"jsonrpc", "2.0"},
                {"method", m_method}
            });

            if (m_paramsOpt.has_value())
            {
                json["params"] = m_paramsOpt.value();
            }

            return json;
        }

        static Request BuildRequest(const std::string& method) noexcept
        {
            return Request(Id::Generate(), method, tl::nullopt);
        }

        static Request BuildRequest(const std::string& method, const json& params) noexcept
        {
            return Request(Id::Generate(), method, tl::make_optional(params));
        }

        Response BuildResult(const json& result) const noexcept
        {
            return Response::BuildResult(m_id, result);
        }

        Response BuildError(const int code, const std::string& message, const tl::optional<json>& data = tl::nullopt) const noexcept
        {
            return Response::BuildError(m_id, code, message, data);
        }

        // TODO: Use enum and lookup id, type, and message
        Response BuildError(const std::string& type, const std::string& message) const noexcept
        {
            return Response::BuildError(m_id, -100, message, json({ {"type", type} }));
        }

        Response BuildError(const Error& error) const noexcept
        {
            return Response::BuildError(m_id, error.GetCode(), error.GetMsg(), error.GetData());
        }

        const Id& GetId() const noexcept { return m_id; }
        const std::string& GetMethod() const noexcept { return m_method; }
        const tl::optional<json>& GetParams() const noexcept { return m_paramsOpt; }

    private:
        Request(const Id& id, const std::string& method, tl::optional<json>&& paramsOpt)
            : m_id(id), m_method(method), m_paramsOpt(std::move(paramsOpt))
        {

        }

        Id m_id;
        std::string m_method;
        tl::optional<json> m_paramsOpt;
    };
}