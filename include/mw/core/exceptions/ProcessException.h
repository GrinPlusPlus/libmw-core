#pragma once

#include<mw/core/exceptions/GrinException.h>
#include<mw/core/util/StringUtil.h>

#define ProcessEx(msg) ProcessException(msg, __FUNCTION__)
#define ProcessEx_F(msg, ...) ProcessException(StringUtil::Format(msg, __VA_ARGS__), __FUNCTION__)

class ProcessException : public GrinException
{
public:
    ProcessException(const std::string& message, const std::string& function)
        : GrinException("ProcessException", message, function)
    {

    }
};