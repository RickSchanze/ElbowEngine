/**
 * @file CallStackFrame.cpp
 * @author Echo 
 * @Date 24-10-26
 * @brief 
 */
#include "CallStackFrame.h"
#include <cpptrace.hpp>

Array<core::CallStackFrame> core::GetCurrentStackTrace()
{
    auto                  trace = cpptrace::generate_trace();
    Array<CallStackFrame> frames;
    for (const auto& frame: trace.frames)
    {
        frames.emplace_back(frame.symbol, frame.filename, frame.line.value_or(-1));
    }
    return frames;
}
