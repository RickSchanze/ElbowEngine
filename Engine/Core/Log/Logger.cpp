/**
 * @file Logger.cpp
 * @author Echo 
 * @Date 24-4-6
 * @brief 
 */

#include "Logger.h"

#include "CoreLogCategory.h"
#include "LogEvent.h"
#include "LogRecorder.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace core
{

static void LogErrorStack(core::Log& log)
{
    if (log.level == LogLevel::Error)
    {
        String outStack = "Call Stack: \n";
        for (auto element: log.call_stack)
        {
            outStack += fmt::format("  {}:{} {}\n", element.file, element.line, element.function);
        }
        LOGGER.ErrorFast(log.category, "{}", outStack);
    }
}

Logger::Logger()
{
    auto color_logger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>(spdlog::color_mode::always);

    logger_ = MakeUnique<spdlog::logger>("ElbowEngine", spdlog::sinks_init_list{color_logger});
    logger_->set_pattern("[%Y-%m-%d %H:%M:%S] [thread: %t] [%l] %v");
    Event_OnLog.AddBind(&LogErrorStack);
}

void Logger::LogStackTrace(LogLevel level)
{
    auto trace = GetCurrentStackTrace();
    if (trace.empty())
    {
        return;
    }
    core::String msg = "Stack Trace: \n";
    int          i   = 0;
    for (auto frame: trace)
    {
        msg += fmt::format("  #{} {}:{} {}\n", i++, frame.file, frame.line, frame.function);
    }
    switch (level)
    {
    case LogLevel::Debug: this->Debug(logcat::StackTrace, "{}", msg); break;
    case LogLevel::Info: this->Info(logcat::StackTrace, "{}", msg); break;
    case LogLevel::Warn: this->Warn(logcat::StackTrace, "{}", msg); break;
    case LogLevel::Error: this->Error(logcat::StackTrace, "{}", msg); break;
    case LogLevel::Critical: this->Critical(logcat::StackTrace, "{}", msg); break;
    case LogLevel::Count: break;
    }
}

void Logger::SendLog(LogLevel level, const core::StringView category, const core::StringView msg)
{
    Log log;
    log.level      = level;
    log.category   = category;
    log.message    = msg;
    log.thread_id  = core::GetCurrentThreadId();
    log.time       = std::chrono::steady_clock::now();
    log.call_stack = core::GetCurrentStackTrace();
    Event_OnLog.Invoke(log);
}

Logger& GetLogger()
{
    static Logger logger;
    return logger;
}

}   // namespace core
