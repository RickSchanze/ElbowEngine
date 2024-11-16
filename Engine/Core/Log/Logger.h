/**
 * @file Logger.h
 * @author Echo 
 * @Date 24-4-6
 * @brief 
 */
#pragma once

#define SPDLOG_WCHAR_TO_UTF8_SUPPORT

#include "Core/Base/EString.h"
#include "Core/Base/UniquePtr.h"
#include "spdlog/spdlog.h"

namespace core
{
enum class LogLevel
{
    Debug,
    Info,
    Warn,
    Error,
    Critical,
    Count,
};

class Logger
{
public:
    Logger();

    template<typename... Args>
    void Debug(const core::StringView category, const spdlog::format_string_t<Args...> msg, Args&&... args)
    {
        String message = fmt::format(msg, std::forward<Args>(args)...);
        logger_->debug("[{}] {}", category.Data(), message);
        SendLog(LogLevel::Debug, category, message);
    }

    template<typename... Args>
    void DebugFast(const core::StringView category, const spdlog::format_string_t<Args...> msg, Args&&... args)
    {
        logger_->debug("[{}] {}", category.Data(), fmt::format(msg, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void Info(const core::StringView category, const spdlog::format_string_t<Args...> msg, Args&&... args)
    {
        String message = fmt::format(msg, std::forward<Args>(args)...);
        logger_->info("[{}] {}", category.Data(), message);
        SendLog(LogLevel::Info, category, message);
    }

    template<typename... Args>
    void InfoFast(const core::StringView category, const spdlog::format_string_t<Args...> msg, Args&&... args)
    {
        logger_->info("[{}] {}", category.Data(), fmt::format(msg, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void Warn(const core::StringView category, const spdlog::format_string_t<Args...> msg, Args&&... args)
    {
        String message = fmt::format(msg, std::forward<Args>(args)...);
        logger_->warn("[{}] {}", category.Data(), message);
        SendLog(LogLevel::Warn, category, message);
    }

    template<typename... Args>
    void WarnFast(const core::StringView category, const spdlog::format_string_t<Args...> msg, Args&&... args)
    {
        logger_->warn("[{}] {}", category.Data(), fmt::format(msg, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void Error(const core::StringView category, const spdlog::format_string_t<Args...> msg, Args&&... args)
    {
        String message = fmt::format(msg, std::forward<Args>(args)...);
        logger_->error("[{}] {}", category.Data(), message);
        SendLog(LogLevel::Error, category, message);
    }

    template<typename... Args>
    void ErrorFast(const core::StringView category, const spdlog::format_string_t<Args...> msg, Args&&... args)
    {
        String message = fmt::format(msg, std::forward<Args>(args)...);
        logger_->error("[{}] {}", category.Data(), message);
    }

    template<typename... Args>
    void Critical(const core::StringView category, const spdlog::format_string_t<Args...> msg, Args&&... args)
    {
        String message = fmt::format(msg, std::forward<Args>(args)...);
        logger_->critical("[{}] {}", category.Data(), message);
        SendLog(LogLevel::Critical, category, message);
    }

private:
    void LogStackTrace(LogLevel level);
    void SendLog(LogLevel level, core::StringView category, core::StringView msg);

    UniquePtr<spdlog::logger> logger_;
};

Logger& GetLogger();
}   // namespace core

#define LOGGER core::GetLogger()
