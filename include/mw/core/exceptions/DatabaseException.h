#pragma once

#include <mw/core/exceptions/GrinException.h>
#include <mw/core/util/StringUtil.h>

#define ThrowDatabase(msg) throw DatabaseException(msg, __FUNCTION__)
#define ThrowDatabase_F(msg, ...) throw DatabaseException(StringUtil::Format(msg, __VA_ARGS__), __FUNCTION__)

class DatabaseException : public GrinException
{
public:
    DatabaseException(const std::string& message, const std::string& function)
        : GrinException("DatabaseException", message, function)
    {

    }
};