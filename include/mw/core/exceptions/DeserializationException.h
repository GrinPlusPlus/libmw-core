#pragma once

#include <mw/core/exceptions/GrinException.h>
#include <mw/core/util/StringUtil.h>

#define ThrowDeserialization(msg) throw DeserializationException(msg, __FUNCTION__)
#define ThrowDeserialization_F(msg, ...) throw DeserializationException(StringUtil::Format(msg, __VA_ARGS__), __FUNCTION__)

class DeserializationException : public GrinException
{
public:
    DeserializationException(const std::string& message, const std::string& function)
        : GrinException("DeserializationException", message, function)
    {

    }
};