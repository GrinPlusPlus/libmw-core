#include <mw/core/net/proxy/Socks5Proxy.h>
#include <mw/core/common/Logger.h>
#include <mw/core/exceptions/NetworkException.h>
#include <cassert>
#include <chrono>

static const std::chrono::seconds SOCKS_TIMEOUT = std::chrono::seconds(3);

Socks5Proxy::Ptr Socks5Proxy::Connect(
    const SocketAddress& proxyAddress,
    const std::string& destination,
    const uint16_t port,
    const tl::optional<SOCKS::ProxyCredentials>& authOpt)
{
    if (destination.size() > 255)
    {
        ThrowNetwork_F("Destination too long: {}", destination);
    }

    if (authOpt.has_value())
    {
        if (authOpt.value().username.size() > 255 || authOpt.value().password.size() > 255)
        {
            ThrowNetwork("Proxy username or password too long");
        }
    }

    for (size_t i = 0; i < 3; i++)
    {
        Socket::Ptr pSocket = Socket::Connect(proxyAddress, std::chrono::seconds(10));

        try
        {
            auto ptr = std::make_shared<Socks5Proxy>(Socks5Proxy(pSocket));
            ptr->Connect(destination, port, authOpt);
            return ptr;
        }
        catch (std::exception & e)
        {
            LOG_WARNING_F("Attempt {}: Failed to connect to {}. Error: {}", i + 1, destination, e.what());
        }
    }

    ThrowNetwork_F("Failed to connect to {}:{} using proxy {}", destination, port, proxyAddress);
}

void Socks5Proxy::Connect(const std::string& destination, const uint16_t port, const tl::optional<SOCKS::ProxyCredentials>& authOpt)
{
    assert(destination.size() <= 255);
    Initialize(authOpt);
    Authenticate(authOpt);

    Serializer serializer;
    serializer.Append<uint8_t>(SOCKS::Version::SOCKS5); // VER protocol version
    serializer.Append<uint8_t>(SOCKS::Command::CONNECT); // CMD CONNECT
    serializer.Append<uint8_t>(0x00); // RSV Reserved must be 0
    serializer.Append<uint8_t>(SOCKS::Atyp::DOMAINNAME); // ATYP DOMAINNAME
    serializer.Append(destination); // Length must be <= 255
    serializer.Append<uint16_t>(port);
    m_pSocket->Write(serializer.vec(), SOCKS_TIMEOUT);

    std::array<uint8_t, 4> connectResponse = m_pSocket->ReadArray<4>(SOCKS_TIMEOUT);
    if (connectResponse[0] != SOCKS::Version::SOCKS5)
    {
        ThrowNetwork("Proxy failed to accept request");
    }

    if (connectResponse[1] != SOCKS::Reply::SUCCEEDED)
    {
        // Failures to connect to a peer that are not proxy errors
        ThrowNetwork_F("Connect failed with error: {}", GetErrorString(connectResponse[1]));
    }

    if (connectResponse[2] != 0x00) // Reserved field must be 0
    {
        ThrowNetwork("Error: malformed proxy response");
    }

    m_destination = ReadDestination((SOCKS::Atyp)connectResponse[3]);
}

void Socks5Proxy::Initialize(const tl::optional<SOCKS::ProxyCredentials>& authOpt)
{
    std::vector<uint8_t> data;
    if (authOpt.has_value())
    {
        data = std::vector<uint8_t>({
            SOCKS::Version::SOCKS5,
            0x02,
            SOCKS::Method::NOAUTH,
            SOCKS::Method::USER_PASS
        });
    }
    else
    {
        data = std::vector<uint8_t>({
            SOCKS::Version::SOCKS5,
            0x01,
            SOCKS::Method::NOAUTH
        });
    }

    m_pSocket->Write(data, SOCKS_TIMEOUT);
}

void Socks5Proxy::Authenticate(const tl::optional<SOCKS::ProxyCredentials>& authOpt)
{
    std::array<uint8_t, 2> received = m_pSocket->ReadArray<2>(SOCKS_TIMEOUT);
    if (received[0] != SOCKS::Version::SOCKS5)
    {
        ThrowNetwork("Proxy failed to initialize");
    }

    if (received[1] == SOCKS::Method::USER_PASS && authOpt.has_value())
    {
        const SOCKS::ProxyCredentials& auth = authOpt.value();
        assert(auth.username.size() <= 255);
        assert(auth.password.size() <= 255);

        // Perform username/password authentication (as described in RFC1929)
        Serializer serializer;
        serializer.Append<uint8_t>(0x01); // Current (and only) version of user/pass subnegotiation
        serializer.Append(auth.username);
        serializer.Append(auth.password);
        m_pSocket->Write(serializer.vec(), SOCKS_TIMEOUT);

        std::array<uint8_t, 2> authResponse = m_pSocket->ReadArray<2>(SOCKS_TIMEOUT);
        if (authResponse[0] != 0x01 || authResponse[1] != 0x00)
        {
            ThrowNetwork("Proxy authentication unsuccessful");
        }
    }
    else if (received[1] == SOCKS::Method::NOAUTH)
    {
        // Perform no authentication
        return;
    }
    else
    {
        ThrowNetwork("Proxy requested wrong authentication method");
    }
}

/** Convert SOCKS5 reply to an error message */
std::string Socks5Proxy::GetErrorString(const uint8_t err)
{
    switch (err)
    {
        case SOCKS::Reply::GENFAILURE:
            return "general failure";
        case SOCKS::Reply::NOTALLOWED:
            return "connection not allowed";
        case SOCKS::Reply::NETUNREACHABLE:
            return "network unreachable";
        case SOCKS::Reply::HOSTUNREACHABLE:
            return "host unreachable";
        case SOCKS::Reply::CONNREFUSED:
            return "connection refused";
        case SOCKS::Reply::TTLEXPIRED:
            return "TTL expired";
        case SOCKS::Reply::CMDUNSUPPORTED:
            return "protocol error";
        case SOCKS::Reply::ATYPEUNSUPPORTED:
            return "address type not supported";
        default:
            return "unknown";
    }
}

SOCKS::Destination Socks5Proxy::ReadDestination(const SOCKS::Atyp& type)
{
    SOCKS::Destination destination;

    switch (type)
    {
        case SOCKS::Atyp::IPV4:
        {
            destination.ipAddress = std::make_optional(IPAddress::CreateV4(m_pSocket->ReadArray<4>(SOCKS_TIMEOUT)));
            break;
        }
        case SOCKS::Atyp::IPV6:
        {
            destination.ipAddress = std::make_optional(IPAddress::CreateV6(m_pSocket->ReadArray<16>(SOCKS_TIMEOUT)));
            break;
        }
        case SOCKS::Atyp::DOMAINNAME:
        {
            std::vector<uint8_t> length = m_pSocket->Read(1, SOCKS_TIMEOUT);
            std::vector<uint8_t> address = m_pSocket->Read(length[0], SOCKS_TIMEOUT);

            destination.domainName = std::make_optional(std::string(address.begin(), address.end()));
            break;
        }
        default:
        {
            ThrowNetwork("Error: malformed proxy response");
        }
    }

    std::vector<uint8_t> portBytes = m_pSocket->Read(2, SOCKS_TIMEOUT);
    destination.port = Deserializer(std::move(portBytes)).Read<uint16_t>();

    return destination;
}