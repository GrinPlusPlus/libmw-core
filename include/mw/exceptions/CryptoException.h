#pragma once

#include <mw/exceptions/GrinException.h>
#include <mw/util/StringUtil.h>

#define CryptoEx(msg) CryptoException(msg, __FUNCTION__)
#define CryptoEx_F(msg, ...) CryptoException(StringUtil::Format(msg, __VA_ARGS__), __FUNCTION__)

class CryptoException : public GrinException
{
public:
    CryptoException(const std::string& message, const std::string& function)
        : GrinException(message, function)
    {

    }
};