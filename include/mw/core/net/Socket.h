#pragma once

#include <asio.hpp>
#include <mw/core/models/net/SocketAddress.h>
#include <functional>
#include <cassert>
#include <string>

//
// This class manages socket timeouts by running the io_context using the timed
// io_context::run_for() member function. Each asynchronous operation is given
// a timeout within which it must complete. The socket operations themselves
// use boost::lambda function objects as completion handlers. For a given
// socket operation, the client object runs the io_context to block thread
// execution until the operation completes or the timeout is reached. If the
// io_context::run_for() function times out, the socket is closed and the
// outstanding asynchronous operation is cancelled.
//
class Socket
{
public:
    using Ptr = std::shared_ptr<Socket>;

    static Socket::Ptr Connect(const SocketAddress& address, const std::chrono::steady_clock::duration& timeout);
    static Socket::Ptr Accept(std::shared_ptr<asio::io_context> pContext, asio::ip::tcp::acceptor& acceptor);

    std::string ReadLine(const std::chrono::steady_clock::duration& timeout)
    {
        // Start the asynchronous operation. The boost::lambda function object is
        // used as a callback and will update the ec variable when the operation
        // completes. The blocking_udp_client.cpp example shows how you can use
        // boost::bind rather than boost::lambda.
        std::size_t n;
        asio::error_code ec;
        asio::async_read_until(*m_pSocket,
            asio::dynamic_buffer(m_stringBuffer),
            '\n', std::bind(&Socket::HandleReceive, std::placeholders::_1, std::placeholders::_2, &ec, &n));

        // Run the operation until it completes, or until the timeout.
        run(timeout);

        // Determine whether the read completed successfully.
        if (ec)
        {
            throw asio::system_error(ec);
        }

        std::string line(m_stringBuffer.substr(0, n - 1));
        m_stringBuffer.erase(0, n);
        return line;
    }

    std::vector<uint8_t> Read(const size_t numBytes, const std::chrono::steady_clock::duration& timeout)
    {
        std::vector<uint8_t> bytes;
        if (m_stringBuffer.size() >= numBytes)
        {
            std::string line(m_stringBuffer.substr(0, numBytes));
            m_stringBuffer.erase(0, numBytes);
            return std::vector<uint8_t>(line.begin(), line.end());
        }

        // Start the asynchronous operation. The boost::lambda function object is
        // used as a callback and will update the ec variable when the operation
        // completes. The blocking_udp_client.cpp example shows how you can use
        // boost::bind rather than boost::lambda.
        std::size_t n;
        asio::error_code ec;
        asio::async_read(*m_pSocket,
            asio::dynamic_buffer(m_stringBuffer),
            asio::transfer_exactly(numBytes - m_stringBuffer.size()),
            std::bind(&Socket::HandleReceive, std::placeholders::_1, std::placeholders::_2, &ec, &n));

        // Run the operation until it completes, or until the timeout.
        run(timeout);

        // Determine whether the read completed successfully.
        if (ec)
        {
            throw asio::system_error(ec);
        }

        std::string line(m_stringBuffer.substr(0, numBytes));
        m_stringBuffer.erase(0, numBytes);
        return std::vector<uint8_t>(line.begin(), line.end());
    }

    template<size_t size>
    std::array<uint8_t, size> ReadArray(const std::chrono::steady_clock::duration& timeout)
    {
        std::vector<uint8_t> bytes = Read(size, timeout);
        assert(bytes.size() == size);

        std::array<uint8_t, size> arr;
        std::move(bytes.begin(), bytes.begin() + size, arr.begin());
        return arr;
    }

    template<class T>
    void Write(const T& data, const std::chrono::steady_clock::duration& timeout)
    {
        // Start the asynchronous operation. The boost::lambda function object is
        // used as a callback and will update the ec variable when the operation
        // completes. The blocking_udp_client.cpp example shows how you can use
        // boost::bind rather than boost::lambda.
        asio::error_code ec;
        asio::async_write(*m_pSocket, asio::buffer(data),
            std::bind(&Socket::HandleConnect, std::placeholders::_1, &ec));

        // Run the operation until it completes, or until the timeout.
        run(timeout);

        // Determine whether the read completed successfully.
        if (ec)
        {
            throw asio::system_error(ec);
        }
    }

private:
    Socket(std::shared_ptr<asio::io_context> pContext, std::shared_ptr<asio::ip::tcp::socket> pSocket)
        : m_pContext(pContext), m_pSocket(pSocket)
    {
    
    }

    void run(const std::chrono::steady_clock::duration& timeout)
    {
        // Restart the io_context, as it may have been left in the "stopped" state
        // by a previous operation.
        m_pContext->restart();

        // Block until the asynchronous operation has completed, or timed out. If
        // the pending asynchronous operation is a composed operation, the deadline
        // applies to the entire operation, rather than individual operations on
        // the socket.
        m_pContext->run_for(timeout);

        // If the asynchronous operation completed successfully then the io_context
        // would have been stopped due to running out of work. If it was not
        // stopped, then the io_context::run_for call must have timed out.
        if (!m_pContext->stopped()) {
            // Close the socket to cancel the outstanding asynchronous operation.
            m_pSocket->close();

            // Run the io_context again until the operation completes.
            m_pContext->run();
        }
    }

    static void HandleConnect(const asio::error_code& ec, asio::error_code* out_ec)
    {
        *out_ec = ec;
    }

    static void HandleReceive(const asio::error_code& ec, std::size_t length, asio::error_code* out_ec, std::size_t* out_length)
    {
        *out_ec = ec;
        *out_length = length;
    }

    std::shared_ptr<asio::io_context> m_pContext;
    std::shared_ptr<asio::ip::tcp::socket> m_pSocket;
    std::string m_stringBuffer;
};