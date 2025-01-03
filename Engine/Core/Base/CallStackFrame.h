/**
 * @file CallStackFrame.h
 * @author Echo 
 * @Date 24-10-26
 * @brief 
 */

#pragma once
#include "Core/CoreDef.h"
#include "EString.h"
namespace core
{
struct CallStackFrame
{
    int32_t line = -1;
    String  file;
    String  function;

    CallStackFrame(StringView func, StringView file, int32_t line) : line(line), file(file), function(func) {}
};

Array<CallStackFrame> GetCurrentStackTrace();

core::String FormatStackTrace(const Array<CallStackFrame>& frames);

}   // namespace core
