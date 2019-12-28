#pragma once

#include <mw/exceptions/GrinException.h>
#include <mw/util/StringUtil.h>

#define DeserializationEx(msg) DeserializationException(msg, __FUNCTION__)
#define DeserializationEx_F(msg, ...) DeserializationException(StringUtil::Format(msg, __VA_ARGS__), __FUNCTION__)

class DeserializationException : public GrinException
{
public:
    DeserializationException(const std::string& message, const std::string& function)
        : GrinException("DeserializationException", message, function)
    {

    }
};