/**
 * @file Exception.h
 * @author Echo 
 * @Date 24-4-7
 * @brief 
 */

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>

#include "Core/CoreDef.h"

class Exception : public std::exception {
public:
    explicit Exception(const StringView View) {
        mStackTrace = cpptrace::generate_trace();
        mMessage    = View;
    }

    [[nodiscard]] virtual StringView What() const { return mMessage; }
    [[nodiscard]] std::string GetStackTrace() const { return mStackTrace.to_string(); }

private:
    [[nodiscard]] char const* what() const override { return ""; }

    StringView mMessage;
    cpptrace::stacktrace mStackTrace;
};

#endif   //EXCEPTION_H
