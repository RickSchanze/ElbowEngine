/**
 * @file CallStackFrame.cpp
 * @author Echo 
 * @Date 24-10-26
 * @brief 
 */
#include "CallStackFrame.h"
#include <cpptrace.hpp>

core::Array<core::CallStackFrame> core::GetCurrentStackTrace()
{
    auto                  trace = cpptrace::generate_trace();
    Array<CallStackFrame> frames;
    for (const auto& frame: trace.frames)
    {
        frames.emplace_back(frame.symbol, frame.filename, frame.line.value_or(-1));
    }
    return frames;
}

static core::StringView ExtractEngineSourceFile(core::StringView file_name)
{
    if (file_name.ContainsAny("/ElbowEngine/Engine/"))
    {
        return file_name.SubString(file_name.LastIndexOf("ElbowEngine/Engine/"));
    }
    if (file_name.ContainsAny(R"(\ElbowEngine\Engine\)"))
    {
        return file_name.SubString(file_name.LastIndexOf(R"(\ElbowEngine\Engine\)"));
    }
    return file_name;
}

core::String core::FormatStackTrace(const Array<CallStackFrame>& frames)
{
    String result = "调用栈: \n";
    for (const auto& frame: frames)
    {
        result += fmt::format("  {}:{} {}\n", ExtractEngineSourceFile(frame.file), frame.line, frame.function);
    }
    return result;
}
