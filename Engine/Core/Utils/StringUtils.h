/**
 * @file StringUtils.h
 * @author Echo 
 * @Date 24-4-6
 * @brief 
 */

#pragma once

#include "CoreDef.h"
#include "CoreTypeTraits.h"

enum class EStringEncoding {
    UTF8,
    GBK,
};


class StringUtils {
public:
    /**
     * 将String(std::wstring)转为std::string
     * @note Windows下使用 因为使用了WideCharToMultiByte
     * @param Str
     * @return
     */
    static std::string ToAnsiString(const String& Str);

    /**
     * 将std::string转换为String(std::wstring)
     * @note Windows下使用 因为使用了MultiByteToWideChar
     * @param Str
     * @param SourceStringEncoding Str的编码
     * @return
     */
    static String FromAnsiString(
        const std::string& Str, EStringEncoding SourceStringEncoding = EStringEncoding::UTF8
    );

    template<typename T>
        requires IsNumeric<T>
    static Optional<T> ConvertTo(const AnsiString& Str) {
        T                Result;
        AnsiStringStream ss;
        ss << Str;
        ss >> Result;
        if (ss.fail() || !ss.eof()) {
            return std::nullopt;
        }
        return Result;
    }
};

template<>
inline Optional<bool> StringUtils::ConvertTo<bool>(const AnsiString& Str) {
    Optional<bool> Rtn;
    if (Str == "true" || Str == "True") {
        Rtn = true;
    } else if (Str == "false" || Str == "False") {
        Rtn = false;
    }
    return Rtn;
}
