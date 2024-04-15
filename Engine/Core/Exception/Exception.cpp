/**
 * @file Exception.cpp
 * @author Echo 
 * @Date 24-4-7
 * @brief 
 */

#include "Exception.h"

#include "Utils/StringUtils.h"

#include <format>

String FileOpenException::What() const {
    return std::format(
        L"FileOpenException: {}\n"
        L"  错误码: {}\n"
        L"  错误消息: {}\n"
        L"  文件名: {}",
        mMessage,
        Errno,
        StringUtils::FromAnsiString(strerror(Errno)),
        Filepath
    );
}
