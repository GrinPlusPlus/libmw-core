#pragma once

#include <mw/core/models/net/IPAddress.h>
#include <mw/core/traits/Printable.h>
#include <mw/core/traits/Serializable.h>

#include <asio.hpp>

class SocketAddress : public Traits::IPrintable, public Traits::ISerializable
{
public:
    //
    // Constructors
    //
    SocketAddress(IPAddress&& ipAddress, const uint16_t port)  : m_ipAddress(std::move(ipAddress)), m_port(port) { }
    SocketAddress(const IPAddress& ipAddress, const uint16_t port) : m_ipAddress(ipAddress), m_port(port) { }
    SocketAddress(const std::string& ipAddress, const uint16_t port) : m_ipAddress(IPAddress::Parse(ipAddress)), m_port(port) { }
    SocketAddress(const SocketAddress& other) = default;
    SocketAddress(SocketAddress&& other) noexcept = default;

    //
    // Destructor
    //
    virtual ~SocketAddress() = default;

    //
    // Operators
    //
    SocketAddress& operator=(const SocketAddress& other) = default;
    SocketAddress& operator=(SocketAddress&& other) noexcept = default;

    //
    // Getters
    //
    const IPAddress& GetIPAddress() const noexcept { return m_ipAddress; }
    const uint16_t GetPortNumber() const noexcept { return m_port; }
    asio::ip::tcp::endpoint GetEndpoint() const noexcept { return asio::ip::tcp::endpoint(m_ipAddress.GetAddress(), m_port); }

    virtual std::string Format() const override final
    {
        return m_ipAddress.Format() + ":" + std::to_string(m_port);
    }

    //
    // Serialization/Deserialization
    //
    virtual Serializer& Serialize(Serializer& serializer) const override final
    {
        return m_ipAddress.Serialize(serializer)
            .Append<uint16_t>(m_port);
    }

    static SocketAddress Deserialize(ByteBuffer& byteBuffer)
    {
        IPAddress ipAddress = IPAddress::Deserialize(byteBuffer);
        const uint16_t port = byteBuffer.ReadU16();

        return SocketAddress(std::move(ipAddress), port);
    }

private:
    IPAddress m_ipAddress;
    uint16_t m_port;
};