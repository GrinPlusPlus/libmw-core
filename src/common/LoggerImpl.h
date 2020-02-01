#pragma once

#include <spdlog/spdlog.h>
#include <mw/core/common/Logger.h>

class Logger
{
public:
    static Logger& GetInstance();

    void StartLogger(
        const FilePath& logDirectory,
        const spdlog::level::level_enum& logLevel
    );

    void Log(
        const LoggerAPI::LogFile file,
        const LoggerAPI::LogLevel logLevel,
        const std::string& eventText
    ) noexcept;

    LoggerAPI::LogLevel GetLogLevel(const LoggerAPI::LogFile file) const noexcept;
    void Flush();

private:
    Logger() = default;

    std::shared_ptr<spdlog::logger> GetLogger(const LoggerAPI::LogFile file) noexcept;
    std::shared_ptr<const spdlog::logger> GetLogger(const LoggerAPI::LogFile file) const noexcept;

    static spdlog::level::level_enum Convert(LoggerAPI::LogLevel logLevel) noexcept;
    static LoggerAPI::LogLevel Convert(spdlog::level::level_enum  logLevel) noexcept;

    std::shared_ptr<spdlog::logger> m_pNodeLogger;
    std::shared_ptr<spdlog::logger> m_pWalletLogger;
};