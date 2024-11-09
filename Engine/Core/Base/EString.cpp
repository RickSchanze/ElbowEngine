/**
 * @file EString.cpp
 * @author Echo 
 * @Date 24-10-26
 * @brief 
 */

#include "EString.h"
#include "CoreDef.h"
#include "unicode/unistr.h"

#include <ostream>
#include <string_view>

namespace core
{

String::String(const StringView& view) : str_(view.Data(), view.Length()) {}

int32_t String::Size() const
{
    const icu::UnicodeString str(this->str_.c_str());
    return str.length();
}

int32_t String::Length() const
{
    return static_cast<int32_t>(str_.size());
}

bool String::operator==(const StringView& o) const
{
    if (Length() != o.Length()) return false;
    int32_t length = ::std::min(Length(), o.Length());
    for (int32_t i = 0; i < length; ++i)
    {
        if (str_[i] != o[i]) return false;
    }
    return true;
}

bool String::Contains(StringView str) const
{
    return str_.find(::std::string_view(str.Data(), str.Length())) != ::std::string::npos;
}

bool String::IsPureSpace() const
{
    return str_.find_first_not_of(' ') == ::std::string::npos;
}

String& String::operator+=(const String& str)
{
    str_ += str.str_;
    return *this;
}

::std::ostream& operator<<(::std::ostream& os, const String& str)
{
    os << str.StdString();
    return os;
}

StringView::StringView(const String& str) : str_(str.str_.c_str()), length_(str.Length()) {}

void StringView::RemoveSubfix(int c)
{
    // DebugAssert
    length_ -= c;
}

bool StringView::IsPureSpace() const
{
    for (int32_t i = 0; i < Length(); ++i)
    {
        if (str_[i] != ' ') return false;
    }
    return true;
}

bool StringView::operator==(const StringView& o) const
{
    if (o.Length() != Length()) return false;
    for (int32_t i = 0; i < Length(); ++i)
    {
        if (str_[i] != o[i]) return false;
    }
    return true;
}

bool StringView::ContainsAny(const StringView& o, bool use_utf8) const
{
    if (use_utf8)
    {
        icu::UnicodeString text    = icu::UnicodeString::fromUTF8(std::string_view(Data(), Length()));
        icu::UnicodeString pattern = icu::UnicodeString::fromUTF8(std::string_view(o.Data(), o.Length()));
        for (int i = 0; i < text.length(); i++)
        {
            UChar32 c = text.char32At(i);
            if (pattern.indexOf(c) >= 0) return true;
        }
        return false;
    }
    else
    {
        for (int i = 0; i < Length(); i++)
        {
            for (int j = 0; j < o.Length(); j++)
                if (str_[i] == o[j]) return true;
        }
        return false;
    }
}

String StringView::operator+(const StringView& o) const
{
    return String(str_) + o;
}

}   // namespace core
