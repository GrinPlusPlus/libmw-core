#pragma once

#include <mw/core/net/Socket.h>
#include <mw/core/net/proxy/Proxy.h>
#include <mw/core/net/proxy/SOCKS.h>
#include <tl/optional.hpp>
#include <memory>

class Socks5Proxy : public IProxy
{
public:
    using Ptr = std::shared_ptr<Socks5Proxy>;

    static Socks5Proxy::Ptr Connect(
        const SocketAddress& proxyAddress,
        const std::string& destination,
        const uint16_t port,
        const tl::optional<SOCKS::ProxyCredentials>& authOpt = tl::nullopt
    );

    virtual ~Socks5Proxy() = default;

private:
    Socket::Ptr m_pSocket;
    SOCKS::Destination m_destination;

    Socks5Proxy(Socket::Ptr pSocket) : m_pSocket(pSocket) { }

    void Connect(
        const std::string& destination,
        const uint16_t port,
        const tl::optional<SOCKS::ProxyCredentials>& authOpt
    );

    void Initialize(
        const tl::optional<SOCKS::ProxyCredentials>& authOpt
    );

    void Authenticate(
        const tl::optional<SOCKS::ProxyCredentials>& authOpt
    );

    SOCKS::Destination ReadDestination(const SOCKS::Atyp& type);

    static std::string GetErrorString(const uint8_t err);
};