//
// Created by Echo on 2025/3/21.
//

#pragma once
#include <filesystem>
#include <source_location>
#include "Core/String/String.hpp"

namespace spdlog {
    class logger;
}
class Logger {
public:
    Logger();

    ~Logger();

    void Info(const std::string &str) const;
    void Warn(const std::string &str) const;
    void Error(const std::string &str) const;
    void Fatal(const std::string &str) const;

private:
    spdlog::logger *logger_;
};

enum LogLevel { Info, Warn, Error, Fatal };

class Log {
public:
    explicit Log(const LogLevel level, const std::source_location &loc = std::source_location::current()) : loc_(loc), level_(level) {}
    explicit Log(const LogLevel level, const StringView name, const std::source_location &loc = std::source_location::current()) :
        loc_(loc), level_(level) {
        manual_func_name = *name;
    }

    void operator<<(const StringView &str) const;

private:
    std::string GetFileName() const;

    std::string_view GetFuncName() const;

    std::source_location loc_;
    const char *manual_func_name = nullptr;
    LogLevel level_;
};
