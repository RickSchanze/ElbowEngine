/**
 * @file LogRecorder.cpp
 * @author Echo 
 * @Date 24-8-27
 * @brief 
 */

#include "LogRecorder.h"

core::LogRecorder g_log_recorder(100);

namespace core
{
constexpr static LogLevel GetLevel(spdlog::level::level_enum level)
{
    switch (level)
    {
    case spdlog::level::debug: return LogLevel::Debug;
    case spdlog::level::info: return LogLevel::Info;
    case spdlog::level::warn: return LogLevel::Warn;
    case spdlog::level::err: return LogLevel::Error;
    case spdlog::level::critical: return LogLevel::Critical;
    default: return LogLevel::Count;
    }
}

LogRecorder::LogRecorder(int32_t max_count)
{
    max_log_counts_ = max_count;
    size_           = 0;
    // TODO: Bind log event
}

void LogRecorder::PushLog(Log& log)
{
    log.index = size_;
    if (size_ != max_log_counts_)
    {
        logs_.push_back(log);
        size_++;
    }
    else
    {
        logs_.pop_front();
        logs_.push_back(log);
    }
}

const List<Log>& LogRecorder::GetLogs() const
{
    return logs_;
}

size_t LogRecorder::GetSize() const
{
    return size_;
}

size_t LogRecorder::GetMaxSize() const
{
    return max_log_counts_;
}

void LogRecorder::Clear()
{
    logs_.clear();
    size_ = 0;
}

void LogRecorder::SetMaxSize(size_t new_size)
{
    if (new_size < max_log_counts_)
    {
        if (size_ > new_size)
        {
            size_t size_to_remove = size_ - new_size;
            for (size_t i = 0; i < size_to_remove; i++)
            {
                logs_.pop_front();
            }
        }
    }
}
}
