/**
 * @file EString.cpp
 * @author Echo 
 * @Date 24-10-26
 * @brief 
 */

#include "EString.h"

#include "Core/CoreDef.h"
#include "Core/Log/CoreLogCategory.h"
#include "Core/Log/Logger.h"
#include "Exception.h"
#include "unicode/unistr.h"

#include <ranges>
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

bool String::EqualsStringView(const StringView& o) const
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

uint32_t String::AtUnicode(int32_t i) const
{
    const icu::UnicodeString str(this->str_.c_str());
    if (i >= str.length()) return 0;
    if (i < 0) return 0;
    return str.char32At(i);
}

UInt64 String::GetHashCode() const
{
    return std::hash<std::string>{}(str_);
}

::std::ostream& operator<<(::std::ostream& os, const String& str)
{
    os << str.GetStdString();
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

StringView StringView::SubString(int32_t begin, int32_t end) const
{
    if (end == -1) end = Length();
    return {str_ + begin, end - begin};
}

int32_t StringView::IndexOf(const StringView& o) const
{
    return static_cast<int32_t>(GetStdStringView().find(o.GetStdStringView()));
}

int32_t StringView::LastIndexOf(const StringView& o) const
{
    return static_cast<int32_t>(GetStdStringView().rfind(o.GetStdStringView()));
}

bool StringView::operator==(const StringView& o) const
{
    if (o.Length() != Length()) return false;
    if (Data() == o.Data()) return true;
    for (int32_t i = 0; i < Length(); ++i)
    {
        if (str_[i] != o[i]) return false;
    }
    return true;
}

bool StringView::operator==(const char* str) const
{
    return GetStdStringView() == ::std::string_view(str);
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

bool StringView::EndsWith(const StringView& o, bool use_utf8) const
{
    if (!use_utf8)
    {
        return Length() >= o.Length() && GetStdStringView().ends_with(o.GetStdStringView());
    }
    // TODO: 实现utf8 ends with
    throw core::NotSupportException("UTF8版本尚未支持");
}

bool StringView::StartsWith(const StringView& o, bool use_utf8) const
{
    if (!use_utf8)
    {
        return Length() >= o.Length() && GetStdStringView().starts_with(o.GetStdStringView());
    }
    // TODO: 实现utf8 starts with
    throw core::NotSupportException("UTF8版本尚未支持");
}

String StringView::operator+(const StringView& o) const
{
    return String(str_) + o;
}

Array<StringView> StringView::Split(const StringView s, const bool utf8_mode) const
{
    Array<StringView> result;
    if (!utf8_mode)
    {
        if (s.IsEmpty() || s == " ")
        {
            size_t     start = 0;
            const auto str   = GetStdStringView();
            while (start < str.size())
            {
                // 跳过前导空白
                while (start < str.size() && std::isspace(static_cast<unsigned char>(str[start])))
                {
                    ++start;
                }

                // 找到下一个空白字符
                size_t end = start;
                while (end < str.size() && !std::isspace(static_cast<unsigned char>(str[end])))
                {
                    ++end;
                }

                // 添加非空白子字符串
                if (start < end)
                {
                    result.emplace_back(str.substr(start, end - start));
                }

                start = end;
            }
        }
        else
        {
            if (s.Length() > 1)
            {
                LOGGER.Warn(logcat::Core, "Split string view only support single char now. use first char.");
            }
            const auto ch    = s[0];
            auto       str   = GetStdStringView();
            size_t     start = 0;
            size_t     end;
            // 循环查找分隔符并截取子字符串
            while ((end = str.find(ch, start)) != std::string_view::npos)
            {
                result.emplace_back(str.substr(start, end - start));
                start = end + 1;   // 跳过分隔符
            }

            // 添加最后一个子字符串（或整个字符串）
            result.emplace_back(str.substr(start));
        }
    }
    else
    {
        // TODO: 实现utf8 split
    }
    return result;
}

StringView StringView::TrimLeft(const StringView s, const bool utf8_mode) const
{
    const auto view = s.GetStdStringView();
    auto       str  = GetStdStringView();
    if (!utf8_mode)
    {
        if (s.IsEmpty())   // trim 空格
        {
            const auto left = std::ranges::find_if(str, [](auto ch) { return !std::isspace(ch); });
            if (left == str.end()) return {};
            return str.substr(left - str.begin());
        }
        else
        {
            if (s.Length() > 1)
            {
                LOGGER.Warn(logcat::Core, "Trim string view only support single char now. use first char.");
            }
            auto       ch   = view[0];
            const auto left = std::ranges::find_if(str, [ch](auto c) { return c != ch; });
            if (left == str.end()) return {};
            return str.substr(left - str.begin());
        }
    }
    else
    {
        // TODO: 实现utf8 trim
        return "";
    }
}

StringView StringView::TrimRight(const StringView s, const bool utf8_mode) const
{
    const auto view = s.GetStdStringView();
    const auto str  = GetStdStringView();
    if (!utf8_mode)
    {
        if (s.IsEmpty())
        {
            size_t end = str.size();
            while (end > 0 && std::isspace(str[end - 1]))
            {
                --end;
            }
            return str.substr(0, end);
        }
        else
        {
            if (s.Length() > 1)
            {
                LOGGER.Warn(logcat::Core, "Trim string view only support single char now. use first char.");
            }
            const auto ch  = view[0];
            size_t     end = str.size();
            while (end > 0 && str[end - 1] == ch)
            {
                --end;
            }
            return str.substr(0, end);
        }
    }
    else
    {
        // TODO: 实现utf8 trim
        return "";
    }
}

StringView StringView::Trim(const StringView s, const bool utf8_mode) const
{
    return TrimLeft(s, utf8_mode).TrimRight(s, utf8_mode);
}

StringView StringView::TrimQuotes() const{
    return Trim("\"");
}

}   // namespace core
