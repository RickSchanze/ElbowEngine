/**
 * @file CachedString.cpp
 * @author Echo 
 * @Date 24-7-26
 * @brief 
 */

#include "CachedString.h"

#include "Utils/StringUtils.h"

String CachedString::ToString()
{
    if (string_.empty())
    {
        string_ = StringUtils::FromAnsiString(ansi_string_);
    }
    return string_;
}

AnsiString CachedString::ToAnsiString()
{
    if (ansi_string_.empty())
    {
        ansi_string_ = StringUtils::ToAnsiString(string_);
    }
    return ansi_string_;
}

const char* CachedString::ToCStyleString()
{
    if (ansi_string_.empty())
    {
        ansi_string_ = StringUtils::ToAnsiString(string_);
    }
    return ansi_string_.c_str();
}

bool CachedString::Empty() const
{
    return string_.empty() && ansi_string_.empty();
}

CachedString::operator bool() const
{
    return !Empty();
}
