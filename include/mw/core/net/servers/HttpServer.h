#pragma once

#include <mw/core/exceptions/NetworkException.h>
#include <mw/core/util/StringUtil.h>
#include <civetweb/civetweb.h>
#include <tl/optional.hpp>
#include <cassert>
#include <string>
#include <memory>


class HttpServer
{
public:
    using Ptr = std::shared_ptr<HttpServer>;

    static HttpServer::Ptr CreateLocal(const tl::optional<uint16_t>& port = tl::nullopt)
    {
        return Create("127.0.0.1", port.value_or(0));
    }

    static HttpServer::Ptr CreatePublic(const tl::optional<uint16_t>& port = tl::nullopt)
    {
        return Create("0.0.0.0", port.value_or(0));
    }

    virtual ~HttpServer()
    {
        mg_stop(m_pContext);
    }

    uint16_t GetPortNumber() const noexcept { return m_portNumber; }

    void AddHandler(const std::string& uri, mg_request_handler handler, void* pCallbackData) noexcept
    {
        mg_set_request_handler(m_pContext, uri.c_str(), handler, pCallbackData);
    }

private:
    static HttpServer::Ptr Create(const std::string& listenerAddr, const uint16_t port)
    {
        std::string listeningPort = StringUtil::Format("{}:{}", listenerAddr, port);

        const char* pOptions[] = {
            "num_threads", "4",
            "listening_ports", listeningPort.c_str(),
            NULL
        };

        auto pCivetContext = mg_start(NULL, 0, pOptions);
        if (pCivetContext == nullptr)
        {
            ThrowNetwork("mg_start failed.");
        }

        mg_server_ports ports;
        if (mg_get_server_ports(pCivetContext, 1, &ports) != 1)
        {
            mg_stop(pCivetContext);
            ThrowNetwork("mg_get_server_ports failed.");
        }

        return std::shared_ptr<HttpServer>(new HttpServer(pCivetContext, (uint16_t)ports.port));
    }

    HttpServer(mg_context* pContext, const uint16_t portNumber)
        : m_pContext(pContext), m_portNumber(portNumber)
    {
        assert(pContext != nullptr);
        assert(portNumber > 0);
    }

    mg_context* m_pContext;
    uint16_t m_portNumber;
};