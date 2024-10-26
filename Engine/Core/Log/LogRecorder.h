/**
 * @file LogRecorder.h
 * @author Echo 
 * @Date 24-8-27
 * @brief 
 */

#pragma once
#include "Base/CallStackFrame.h"
#include "Base/CoreTypeDef.h"
#include "CoreDef.h"
#include "Logger.h"

#include <chrono>

namespace spdlog::details
{
struct log_msg;
}

namespace core
{
struct Log
{
    Array<CallStackFrame> call_stack;
    StringView            category;
    String                message;
    LogLevel              level = LogLevel::Count;
    String                filename;
    String                function;
    int32_t               line      = -1;
    size_t                thread_id = -1;
    TimePoint             time;
    int32_t               index = 0;

    Log() = default;
};

class LogRecorder
{
public:
    explicit LogRecorder(int32_t max_count = 100);
    void     PushLog(Log& log);

    [[nodiscard]] const List<Log>& GetLogs() const;
    [[nodiscard]] size_t           GetSize() const;
    [[nodiscard]] size_t           GetMaxSize() const;
    [[nodiscard]] LogLevel         GetTraceLevel() const { return trace_level_; }

    void Clear();

    /**
     * 为LogRecorder设置一个新的最大size
     * @param new_size 如果new_size < 当前size则会移除前面的Log
     */
    void SetMaxSize(size_t new_size);

private:
    LogLevel  trace_level_ = LogLevel::Count;
    List<Log> logs_;
    int32_t   max_log_counts_;
    int32_t   size_;
};
}   // namespace core

extern core::LogRecorder g_log_recorder;
