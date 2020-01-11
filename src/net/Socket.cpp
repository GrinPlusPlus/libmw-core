#include <mw/core/net/Socket.h>

// TODO: Currently this always creates a real Socket. Need to add support for mock sockets.
Socket::Ptr Socket::Connect(
    const SocketAddress& address,
    const std::chrono::steady_clock::duration& timeout)
{
    auto pContext = std::make_shared<asio::io_context>();
    auto pSocket = std::make_shared<asio::ip::tcp::socket>(*pContext);
    auto ptr = std::shared_ptr<Socket>(new Socket(pContext, pSocket));

    // Start the asynchronous operation itself. The boost::lambda function
    // object is used as a callback and will update the ec variable when the
    // operation completes. The blocking_udp_client.cpp example shows how you
    // can use boost::bind rather than boost::lambda.
    asio::error_code ec;
    ptr->m_pSocket->async_connect(
        address.GetEndpoint(),
        std::bind(&Socket::HandleConnect, std::placeholders::_1, &ec)
    );

    // Run the operation until it completes, or until the timeout.
    ptr->run(timeout);

    // Determine whether a connection was successfully established.
    if (ec)
    {
        throw asio::system_error(ec);
    }

    return ptr;
}

Socket::Ptr Socket::Accept(
    std::shared_ptr<asio::io_context> pContext,
    asio::ip::tcp::acceptor& acceptor)
{
    auto pSocket = std::make_shared<asio::ip::tcp::socket>(*pContext);
    auto ptr = std::shared_ptr<Socket>(new Socket(pContext, pSocket));

    asio::error_code ec;
    acceptor.async_accept(*pSocket, [&ec, pContext](const asio::error_code& error) {
        ec = error;
    });

    ptr->run(std::chrono::milliseconds(100));

    if (ec)
    {
        throw asio::system_error(ec);
    }

    return ptr;
}