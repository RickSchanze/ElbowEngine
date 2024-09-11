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
class CachedString
{
public:


    CachedString() = default;

    CachedString(String string) : string_(std::move(string)) {}
    CachedString(AnsiString string) : ansi_string_(std::move(string)) {}
    CachedString(const wchar_t* string) : string_(string) {}
    CachedString(const char* string) : ansi_string_(string) {}

    operator const AnsiString&();

    String      ToString();
    AnsiString  ToAnsiString();
    const char* ToCStyleString();

    bool Empty() const;

    operator bool() const;

    bool operator==(const CachedString&) const;

private:
    String     string_;
    AnsiString ansi_string_;
};
