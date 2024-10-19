/**
 * @file StringUtils.h
 * @author Echo 
 * @Date 24-4-6
 * @brief 
 */

#pragma once

#include "CoreDef.h"
#include "CoreTypeTraits.h"
#include "Math/MathTypes.h"

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
        const std::string& Str, EStringEncoding SourceStringEncoding = EStringEncoding::UTF8);

    /**
     * 异常码转字符串 调用strerror
     * @param ErrorCode 异常码
     * @param SourceStringEncoding 异常码字符串的编码
     * @return
     */
    static String ErrorCodeToString(errno_t ErrorCode, EStringEncoding SourceStringEncoding = EStringEncoding::GBK);

    static String FromAnsiStringView(const std::string_view& AnsiStrView);
    static String FromAnsiStringView(const rttr::string_view& AnsiStrView);

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

    static String ToString(Vector3 InVec);
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
