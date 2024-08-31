/**
 * @file LogRecorder.cpp
 * @author Echo 
 * @Date 24-8-27
 * @brief 
 */

#include "LogRecorder.h"

#include "LogEvent.h"

LogRecorder g_log_recorder(100);

static void HandleOnLog(const spdlog::details::log_msg& msg)
{
    g_log_recorder.PushLog(msg);
}

Log::Log(const spdlog::details::log_msg& msg, ELogLevel trace_level)
{
    message = AnsiString(msg.payload.begin(), msg.payload.end());
    switch (msg.level)
    {
    case spdlog::level::trace: level = ELogLevel::Trace; break;
    case spdlog::level::debug: level = ELogLevel::Debug; break;
    case spdlog::level::info: level = ELogLevel::Info; break;
    case spdlog::level::warn: level = ELogLevel::Warning; break;
    case spdlog::level::err: level = ELogLevel::Error; break;
    case spdlog::level::critical: level = ELogLevel::Critical; break;
    default: level = ELogLevel::MaxDefault; break;
    }
    // filename  = msg.source.filename;
    // line      = msg.source.line;
    // function  = msg.source.funcname;
    thread_id = msg.thread_id;
    time      = msg.time;

    // 调用栈
    if (level >= trace_level)
    {
        auto trace = cpptrace::generate_trace();
        for (auto& frame: trace.frames)
        {
            CallStackFrame new_frame;
            new_frame.file     = Move(frame.filename);
            new_frame.line     = frame.line.value_or(0);
            new_frame.function = Move(frame.symbol);
            call_stack.push_back(Move(new_frame));
        }
    }
}

LogRecorder::LogRecorder(size_t max_count)
{
    max_log_counts_ = max_count;
    size_           = 0;
    OnLog.Add(&HandleOnLog);
}

void LogRecorder::PushLog(const spdlog::details::log_msg& msg)
{
    auto l = Log(msg, trace_level_);
    l.index = size_;
    if (size_ != max_log_counts_)
    {
        logs_.push_back(Move(l));
        size_++;
    }
    else
    {
        logs_.pop_front();
        logs_.push_back(Move(l));
    }
}

const TList<Log>& LogRecorder::GetLogs() const
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


