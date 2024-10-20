/**
 * @file Logger.cpp
 * @author Echo 
 * @Date 24-4-6
 * @brief 
 */

#include "Logger.h"

#include "spdlog/sinks/callback_sink.h"

#include "LogEvent.h"

Logger::Logger()
{
    auto callback_log = std::make_shared<spdlog::sinks::callback_sink_mt>([](const spdlog::details::log_msg& msg) { OnLog.Invoke(msg); });
    auto color_logger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    logger_ = std::make_shared<spdlog::logger>("ElbowEngine", spdlog::sinks_init_list{callback_log, color_logger});
    logger_->set_pattern("[%Y-%m-%d %H:%M:%S] [thread: %t] [%l] %v");
}
