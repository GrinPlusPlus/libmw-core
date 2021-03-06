#pragma once

#include <mw/core/exceptions/GrinException.h>
#include <mw/core/util/StringUtil.h>

#define ThrowCrypto(msg) throw CryptoException(msg, __FUNCTION__)
#define ThrowCrypto_F(msg, ...) throw CryptoException(StringUtil::Format(msg, __VA_ARGS__), __FUNCTION__)

class CryptoException : public GrinException
{
public:
    CryptoException(const std::string& message, const std::string& function)
        : GrinException("CryptoException", message, function)
    {

    }
};