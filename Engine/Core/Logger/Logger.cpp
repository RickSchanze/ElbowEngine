//
// Created by Echo on 2025/3/25.
//
#include "Logger.hpp"

#include <regex>
#include <stacktrace>

#include "Core/Assert.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include "Core/CoreFeature.hpp"
#include "Core/Misc/Other.hpp"

Logger::Logger() {
    auto color_logger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>(spdlog::color_mode::always);
    auto file_logger = std::make_shared<spdlog::sinks::basic_file_sink_mt>("ElbowEngine.log", true);
    logger_ = new spdlog::logger("ElbowEngine", spdlog::sinks_init_list{color_logger, file_logger});
    logger_->set_pattern("[%Y-%m-%d %H:%M:%S] [thread:%t] [%l] %v");
}

Logger::~Logger() { delete logger_; }

void Logger::Info(const std::string &str) const { logger_->info(str); }

void Logger::Warn(const std::string &str) const { logger_->warn(str); }

void Logger::Error(const std::string &str) const { logger_->error(str); }

void Logger::Fatal(const std::string &str) const {
    logger_->critical(str);
    std::stacktrace trace = std::stacktrace::current();
    logger_->critical(*String::Format("Stack trace:\n{}", std::to_string(trace)));
}

void Log::operator<<(const StringView &str) const {
    static Logger g_logger;
    switch (level_) {
        case Info:
            g_logger.Info(std::format("[{}:{}] [{}] {}", GetFileName(), loc_.line(), GetFuncName(), *str));
            break;
        case Warn:
            g_logger.Warn(std::format("[{}:{}] [{}] {}", GetFileName(), loc_.line(), GetFuncName(), *str));
            break;
        case Error:
            g_logger.Error(std::format("[{}:{}] [{}] {}", GetFileName(), loc_.line(), GetFuncName(), *str));
            break;
        case Fatal:
            g_logger.Fatal(std::format("[{}:{}] [{}] {}", GetFileName(), loc_.line(), GetFuncName(), *str));
            Exit(1);
            break;
    }
}

std::string Log::GetFileName() const {
    if constexpr (LOG_FULL_FILE_PATH) {
        return loc_.file_name();
    } else {
        std::filesystem::path p(loc_.file_name());
        return p.filename().string();
    }
}

std::string_view Log::GetFuncName() const {
    if (manual_func_name != nullptr) return manual_func_name;
    if constexpr (LOG_FULL_FUNCTION_DECL) {
        return loc_.function_name();
    } else {
        std::cmatch match;
        static std::regex pattern(R"((?:__\w+\s+)([\w:]+)(?=\s*\())");
        if (regex_search(loc_.function_name(), match, pattern)) {
            return std::string_view(match[1].first, match[1].second - match[1].first);
        }
        return "__Unknown__";
    }
}
