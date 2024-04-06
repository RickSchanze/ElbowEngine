/**
 * @file StringUtils.cpp
 * @author Echo 
 * @Date 24-4-6
 * @brief 
 */

#include "StringUtils.h"
#include "Core/Log/Logger.h"
#include "Windows.h"

std::string StringUtils::ToStdString(const String& Str) {
    if (Str.empty()) return {std::string()};
    const int SizeNeeded = WideCharToMultiByte(
        CP_UTF8, 0, &Str[0], static_cast<int>(Str.size()), nullptr, 0, nullptr, nullptr
    );
    std::string StrRtn(SizeNeeded, 0);
    WideCharToMultiByte(
        CP_UTF8, 0, &Str[0], static_cast<int>(Str.size()), &StrRtn[0], SizeNeeded, nullptr, nullptr
    );
    return StrRtn;
}

String StringUtils::FromStdString(const std::string& Str) {
    if (Str.empty()) return {String()};
    const int SizeNeeded =
        MultiByteToWideChar(CP_UTF8, 0, Str.c_str(), static_cast<int>(Str.size()), nullptr, 0);
    String StrRtn(SizeNeeded, 0);
    MultiByteToWideChar(
        CP_UTF8, 0, Str.c_str(), static_cast<int>(Str.size()), &StrRtn[0], SizeNeeded
    );
    return StrRtn;
}
