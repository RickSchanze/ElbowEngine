/**
 * @file CachedString.cpp
 * @author Echo 
 * @Date 24-7-26
 * @brief 
 */

#include "CachedString.h"

#include "Utils/StringUtils.h"

CachedString::operator const AnsiString&()
{
    if (ansi_string_.empty())
    {
        ansi_string_ = StringUtils::ToAnsiString(string_);
    }
    return ansi_string_;
}

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

bool CachedString::operator==(const CachedString& other) const
{
    if (!ansi_string_.empty())
    {
        return ansi_string_ == other.ansi_string_;
    }
    if (!string_.empty())
    {
        return string_ == other.string_;
    }
    if (other.Empty())
    {
        return true;
    }
    return false;
}
