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
    void Log(const LoggerAPI::LogFile file, const spdlog::level::level_enum logLevel, const std::string& eventText);
    void Flush();

private:
    Logger() = default;

    std::shared_ptr<spdlog::logger> GetLogger(const LoggerAPI::LogFile file);

    std::shared_ptr<spdlog::logger> m_pNodeLogger;
    std::shared_ptr<spdlog::logger> m_pWalletLogger;
};