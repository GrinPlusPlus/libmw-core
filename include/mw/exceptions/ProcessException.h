#pragma once

#include <mw/exceptions/GrinException.h>
#include <mw/util/StringUtil.h>

#define ProcessEx(msg) ProcessException(msg, __FUNCTION__)
#define ProcessEx_F(msg, ...) ProcessException(StringUtil::Format(msg, __VA_ARGS__), __FUNCTION__)

class ProcessException : public GrinException
{
public:
    ProcessException(const std::string& message, const std::string& function)
        : GrinException(message, function)
    {

    }
};