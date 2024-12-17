//
// Created by Echo on 24-12-16.
//

#include "Exception.h"

#include "CallStackFrame.h"
#include "Core/Core.h"

core::Exception::Exception(const core::String& msg)
{
#if ELBOW_DEBUG
    const auto stack_trace = core::GetCurrentStackTrace();
    const auto str         = FormatStackTrace(stack_trace);
    message_               = msg + "\n" + str;
    DEBUG_BREAK();
#else
    message_ = msg;
#endif
}

core::ArgumentException::ArgumentException(core::StringView arg_name, core::StringView msg) :
    Exception(fmt::format("Argument {} Error: {}", arg_name, msg))
{
}