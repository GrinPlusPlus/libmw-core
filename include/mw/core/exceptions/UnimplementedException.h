#pragma once

#include <mw/core/exceptions/GrinException.h>
#include <mw/core/util/StringUtil.h>

#define ThrowUnimplemented(msg) throw UnimplementedException(msg, __FUNCTION__)
#define ThrowUnimplemented_F(msg, ...) throw UnimplementedException(StringUtil::Format(msg, __VA_ARGS__), __FUNCTION__)

class UnimplementedException : public GrinException
{
public:
    UnimplementedException(const std::string& message, const std::string& function)
        : GrinException("UnimplementedException", message, function)
    {

    }
};