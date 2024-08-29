/**
 * @file LogRecorder.h
 * @author Echo 
 * @Date 24-8-27
 * @brief 
 */

#pragma once
#include "CoreDef.h"

#include <chrono>

namespace spdlog::details
{
struct log_msg;
}

enum class ELogLevel
{
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Critical,
    MaxDefault,
};

struct CallStackFrame
{
    uint32_t   line;
    AnsiString file;
    AnsiString function;
};

struct Log
{
    TArray<CallStackFrame>                             call_stack;
    AnsiString                                         message;
    ELogLevel                                          level;
    AnsiString                                         filename;
    AnsiString                                         function;
    int32_t                                            line;
    size_t                                             thread_id;
    std::chrono::time_point<std::chrono::system_clock> time;

    /**
     *
     * @param msg
     * @param trace_level 大于这个level则生成调用栈
     */
    explicit Log(const spdlog::details::log_msg& msg, ELogLevel trace_level);

    Log() = default;
};

class LogRecorder
{
public:
    explicit LogRecorder(size_t max_count = 100);
    void     PushLog(const spdlog::details::log_msg& msg);

    const TList<Log>& GetLogs() const;
    size_t            GetSize() const;
    size_t            GetMaxSize() const;

    /**
     * 为LogRecorder设置一个新的最大size
     * @param new_size 如果new_size < 当前size则会移除前面的Log
     */
    void SetMaxSize(size_t new_size);

    ELogLevel GetTraceLevel() const { return trace_level_; }

private:
    ELogLevel  trace_level_ = ELogLevel::Warning;
    TList<Log> logs_;
    size_t     max_log_counts_;
    size_t     size_;
};

extern LogRecorder g_log_recorder;
