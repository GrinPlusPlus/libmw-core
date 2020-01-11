#pragma once

#include <mw/core/models/net/IPAddress.h>

#include <optional>

namespace SOCKS
{
    /** SOCKS version */
    enum Version : uint8_t
    {
        SOCKS4 = 0x04,
        SOCKS5 = 0x05
    };

    /** Values defined for METHOD in RFC1928 */
    enum Method : uint8_t
    {
        NOAUTH = 0x00,        //! No authentication required
        GSSAPI = 0x01,        //! GSSAPI
        USER_PASS = 0x02,     //! Username/password
        NO_ACCEPTABLE = 0xff, //! No acceptable methods
    };

    /** Values defined for CMD in RFC1928 */
    enum Command : uint8_t
    {
        CONNECT = 0x01,
        BIND = 0x02,
        UDP_ASSOCIATE = 0x03
    };

    /** Values defined for REP in RFC1928 */
    enum Reply : uint8_t
    {
        SUCCEEDED = 0x00,        //! Succeeded
        GENFAILURE = 0x01,       //! General failure
        NOTALLOWED = 0x02,       //! Connection not allowed by ruleset
        NETUNREACHABLE = 0x03,   //! Network unreachable
        HOSTUNREACHABLE = 0x04,  //! Network unreachable
        CONNREFUSED = 0x05,      //! Connection refused
        TTLEXPIRED = 0x06,       //! TTL expired
        CMDUNSUPPORTED = 0x07,   //! Command not supported
        ATYPEUNSUPPORTED = 0x08, //! Address type not supported
    };

    /** Values defined for ATYPE in RFC1928 */
    enum Atyp : uint8_t
    {
        IPV4 = 0x01,
        DOMAINNAME = 0x03,
        IPV6 = 0x04,
    };

    struct Destination
    {
        std::optional<IPAddress> ipAddress;
        std::optional<std::string> domainName;
        uint16_t port;
    };

    /** Credentials for proxy authentication */
    struct ProxyCredentials
    {
        std::string username;
        std::string password;
    };
}