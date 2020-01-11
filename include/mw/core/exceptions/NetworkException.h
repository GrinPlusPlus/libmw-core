#pragma once

#include <mw/core/exceptions/GrinException.h>
#include <mw/core/util/StringUtil.h>

#define ThrowNetwork(msg) throw NetworkException(msg, __FUNCTION__)
#define ThrowNetwork_F(msg, ...) throw NetworkException(StringUtil::Format(msg, __VA_ARGS__), __FUNCTION__)

class NetworkException : public GrinException
{
public:
    NetworkException(const std::string& message, const std::string& function)
        : GrinException("NetworkException", message, function)
    {

    }
};