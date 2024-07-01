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

    explicit Exception(String Message) : message_(std::move(Message)) {
        stack_trace_ = cpptrace::generate_trace();
    }

    [[nodiscard]] virtual String What() const { return L"Exception"; }
    [[nodiscard]] AnsiString     GetStackTrace() const { return stack_trace_.to_string(); }

protected:
    [[nodiscard]] char const* what() const override { return ""; }

    cpptrace::stacktrace stack_trace_;
    String               message_;
};

class FileOpenException final : public Exception {
public:
    /**
     * @param Filepath 打开失败的文件路径
     * @param errno_in 打开失败的错误码
     * @param message 此错误的具体描述
     */
    explicit FileOpenException(String Filepath, const Int32 errno_in, const String& message = L"") :
        Exception(message), errno_{errno_in}, filepath_{std::move(Filepath)} {}

    [[nodiscard]] String What() const override;
    [[nodiscard]] Int32  GetErrno() const { return errno_; }
    [[nodiscard]] String GetFilepath() const { return filepath_; }

private:
    Int32  errno_;
    String filepath_;
};

class NeverEnterException final : public Exception
{
public:
    explicit NeverEnterException(const String& message) : Exception(message) {}
    explicit NeverEnterException() : NeverEnterException(L"不可调用的代码被执行") {}
};
