/**
 * @file CachedString.h
 * @author Echo 
 * @Date 24-7-26
 * @brief 
 */

#pragma once
#include <utility>

#include "CoreDef.h"

/**
* 同时包含String和其对应的AnsiString(cache)的String
*/
class CachedString {
public:
    CachedString() = default;

    explicit CachedString(String  string) : string_(std::move(string)) {}
    explicit CachedString(AnsiString  string) : ansi_string_(std::move(string)) {}
    explicit CachedString(const wchar_t* string) : string_(string) {}
    explicit CachedString(const char* string) : ansi_string_(string) {}

    String ToString();
    AnsiString ToAnsiString();
    const char* ToCStyleString();

private:
    String string_;
    AnsiString ansi_string_;
};
