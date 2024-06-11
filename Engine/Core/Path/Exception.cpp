/**
 * @file Exception.cpp
 * @author Echo 
 * @Date 24-4-15
 * @brief 
 */
#include "Exception.h"

String PathInvalidException::What() const {
    return std::format(
        L"PathInvalidException: \"{}\": {}",
        mPath.ToString(),
        mMessage.empty() ? L"" : L" " + mMessage
    );
}

String ProjectPathNotValidException::What() const {
    return std::format(
        L"ProjectPathNotValidException: 项目工程路径未设置"
    );
}
String FileCreateFailedException::What() const {
    return std::format(
        L"FileCreateFailedException: 创建文件 \"{}\" 失败.{}",
        mPath.ToString(),
        mMessage.empty() ? L"" : L" " + mMessage
    );
}
