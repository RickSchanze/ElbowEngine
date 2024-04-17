/**
 * @file StringUtils.cpp
 * @author Echo 
 * @Date 24-4-6
 * @brief 
 */

#include "StringUtils.h"
#include "Log/Logger.h"
#include "Windows.h"

AnsiString StringUtils::ToAnsiString(const String& Str) {
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

String StringUtils::FromAnsiString(const std::string& Str, const EStringEncoding SourceStringEncoding) {
    if (Str.empty()) return {String()};
    const int SizeNeeded =
        MultiByteToWideChar(CP_UTF8, 0, Str.c_str(), static_cast<int>(Str.size()), nullptr, 0);
    String StrRtn(SizeNeeded, 0);
    uint32 CodePage = CP_UTF8;
    switch (SourceStringEncoding) {
    case EStringEncoding::UTF8: CodePage = CP_UTF8; break;
    case EStringEncoding::GBK: CodePage = CP_ACP; break;
    }
    MultiByteToWideChar(
        CodePage, 0, Str.c_str(), static_cast<int>(Str.size()), &StrRtn[0], SizeNeeded
    );
    return StrRtn;
}
