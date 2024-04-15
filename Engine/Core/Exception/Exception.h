/**
 * @file Exception.h
 * @author Echo 
 * @Date 24-4-7
 * @brief 
 */

#pragma once

#include <exception>
#include <utility>

#include "CoreDef.h"

class Exception : public std::exception {
public:
    ~Exception() override = default;

    explicit Exception(String Message) : mMessage(std::move(Message)) {
        mStackTrace = cpptrace::generate_trace();
    }

    [[nodiscard]] virtual String What() const { return L"Exception"; }
    [[nodiscard]] AnsiString     GetStackTrace() const { return mStackTrace.to_string(); }

protected:
    [[nodiscard]] char const* what() const override { return ""; }

    cpptrace::stacktrace mStackTrace;
    String               mMessage;
};

class FileOpenException final : public Exception {
public:
    /**
     * @param Filepath 打开失败的文件路径
     * @param Errno 打开失败的错误码
     * @param Message 此错误的具体描述
     */
    explicit FileOpenException(String Filepath, const int32 Errno, const String& Message = L"") :
        Exception(Message), Errno{Errno}, Filepath{std::move(Filepath)} {}

    [[nodiscard]] String What() const override;
    [[nodiscard]] int32  GetErrno() const { return Errno; }
    [[nodiscard]] String GetFilepath() const { return Filepath; }

private:
    int32  Errno;
    String Filepath;
};
