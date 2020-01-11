#pragma once

#include <mw/core/models/net/SocketAddress.h>
#include <mw/core/exceptions/NetworkException.h>
#include <mw/core/net/Socket.h>
#include <asio.hpp>
#include <tl/optional.hpp>
#include <memory>
#include <atomic>

class Server
{
public:
    using Ptr = std::shared_ptr<Server>;

    virtual ~Server() = default;

    static Server::Ptr CreateLocal(const tl::optional<uint16_t>& port = tl::nullopt)
    {
        return Create(SocketAddress(IPAddress::Parse("127.0.0.1"), port.value_or(0)));
    }

    static Server::Ptr CreatePublic(const tl::optional<uint16_t>& port = tl::nullopt)
    {
        return Create(SocketAddress(IPAddress::Parse("0.0.0.0"), port.value_or(0)));
    }
    
private:
    Server(
        std::shared_ptr<asio::io_context> pContext,
        std::shared_ptr<asio::ip::tcp::acceptor> pAcceptor,
        SocketAddress&& address)
        : m_pContext(pContext),
        m_pAcceptor(pAcceptor),
        m_address(std::move(address))
    {

    }

    static Server::Ptr Create(SocketAddress&& address)
    {
        auto pContext = std::make_shared<asio::io_context>();
        auto pAcceptor = std::make_shared<asio::ip::tcp::acceptor>(*pContext, address.GetEndpoint());

        asio::error_code errorCode;
        pAcceptor->listen(asio::socket_base::max_listen_connections, errorCode);
        if (errorCode)
        {
            ThrowNetwork_F("Listener failed with error: {}", errorCode.message());
        }

        Server::Ptr pServer(new Server(pContext, pAcceptor, std::move(address)));
    }

    static void Thread_Loop(Server::Ptr pServer)
    {
        while (!pServer->m_terminate)
        {
            try
            {
                Socket::Ptr pSocket = Socket::Accept(pServer->m_pContext, *pServer->m_pAcceptor);
            }
            catch (std::system_error&)
            {
                //if (e.code() == )
            }
        }

        pServer->m_pAcceptor->cancel();
    }

    std::shared_ptr<asio::io_context> m_pContext;
    std::shared_ptr<asio::ip::tcp::acceptor> m_pAcceptor;
    SocketAddress m_address;

    std::atomic_bool m_terminate;
    std::thread m_thread;
};