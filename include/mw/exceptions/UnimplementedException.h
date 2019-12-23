#pragma once

#include <mw/exceptions/GrinException.h>
#include <mw/util/StringUtil.h>

#define UnimplementedEx(msg) UnimplementedException(msg, __FUNCTION__)
#define UnimplementedEx_F(msg, ...) UnimplementedException(StringUtil::Format(msg, __VA_ARGS__), __FUNCTION__)

class UnimplementedException : public GrinException
{
public:
    UnimplementedException(const std::string& message, const std::string& function)
        : GrinException(message, function)
    {

    }
};