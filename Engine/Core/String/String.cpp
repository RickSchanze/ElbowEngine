/**
 * @file String.cpp
 * @author Echo
 * @Date 24-10-26
 * @brief
 */

#include "String.hpp"
#include "Core/Core.hpp"
#include "Core/Logger/Logger.hpp"
#include "utf8cpp/utf8.h"
#ifdef ELBOW_PLATFORM_WINDOWS
#include <Windows.h>
#undef min
#endif

String::String(const StringView &view) : str_(view.Data(), view.ByteCount()) {}
String::~String() {}

String String::FromWideChar(const std::wstring &wstr) {
    if (wstr.empty())
        return "";
#ifdef ELBOW_PLATFORM_WINDOWS
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
    std::string utf8_str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), &utf8_str[0], size_needed, nullptr, nullptr);
    return utf8_str;
#endif
}

UInt64 String::Count() const { return static_cast<UInt64>(utf8::distance(str_.begin(), str_.end())); }

int32_t String::ByteCount() const { return static_cast<int32_t>(str_.size()); }

bool String::EqualsStringView(const StringView &o) const {
    if (ByteCount() != o.ByteCount())
        return false;
    int32_t length = ::std::min(ByteCount(), o.ByteCount());
    for (int32_t i = 0; i < length; ++i) {
        if (str_[i] != o[i])
            return false;
    }
    return true;
}

bool String::Contains(const StringView str) const { return str_.find(::std::string_view(str.Data(), str.ByteCount())) != ::std::string::npos; }

bool String::IsPureSpace() const { return str_.find_first_not_of(' ') == ::std::string::npos; }

String &String::operator+=(const String &str) {
    str_ += str.str_;
    return *this;
}

UInt64 String::GetHashCode() const { return std::hash<std::string>{}(str_); }

std::wstring String::ToWideString() const {
#ifdef ELBOW_PLATFORM_WINDOWS
    if (str_.empty())
        return L"";
    UInt32 size_needed = MultiByteToWideChar(CP_UTF8, 0, str_.c_str(), str_.size(), nullptr, 0);
    std::wstring utf16_str(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str_.c_str(), static_cast<int>(str_.size()), &utf16_str[0], size_needed);
    return utf16_str;
#else
    return {};
#endif
}

StringView::StringView(const String &str) : str_(str.str_.c_str()), length_(str.ByteCount()) {}

void StringView::RemoveSubfix(const int c) {
    // DebugAssert
    length_ -= c;
}

bool StringView::IsPureSpace() const {
    for (int32_t i = 0; i < ByteCount(); ++i) {
        if (str_[i] != ' ')
            return false;
    }
    return true;
}

StringView StringView::SubString(const int32_t begin, int32_t end) const {
    if (end == -1)
        end = ByteCount();
    return {str_ + begin, end - begin};
}

int32_t StringView::IndexOf(const StringView &o) const { return static_cast<int32_t>(ToStdStringView().find(o.ToStdStringView())); }

int32_t StringView::LastIndexOf(const StringView &o) const { return static_cast<int32_t>(ToStdStringView().rfind(o.ToStdStringView())); }

bool StringView::operator==(const StringView &o) const {
    if (o.ByteCount() != ByteCount())
        return false;
    if (Data() == o.Data())
        return true;
    for (int32_t i = 0; i < ByteCount(); ++i) {
        if (str_[i] != o[i])
            return false;
    }
    return true;
}

bool StringView::operator==(const char *str) const { return ToStdStringView() == ::std::string_view(str); }

bool StringView::ContainsAny(const StringView &o) const {
    for (int i = 0; i < ByteCount(); i++) {
        for (int j = 0; j < o.ByteCount(); j++)
            if (str_[i] == o[j])
                return true;
    }
    return false;
}

bool StringView::EndsWith(const StringView &o) const { return ByteCount() >= o.ByteCount() && ToStdStringView().ends_with(o.ToStdStringView()); }

bool StringView::StartsWith(const StringView &o) const { return ByteCount() >= o.ByteCount() && ToStdStringView().starts_with(o.ToStdStringView()); }

std::wstring StringView::ToWideString() const {
    String str = ToString();
    return str.ToWideString();
}

String StringView::ToString() const { return {str_, length_}; }

String StringView::operator+(const StringView &o) const { return String(str_) + o; }

Array<StringView> StringView::Split(const StringView s) const {
    Array<StringView> result;
    if (s.IsEmpty() || s == " ") {
        size_t start = 0;
        const auto str = ToStdStringView();
        while (start < str.size()) {
            // 跳过前导空白
            while (start < str.size() && std::isspace(static_cast<unsigned char>(str[start]))) {
                ++start;
            }

            // 找到下一个空白字符
            size_t end = start;
            while (end < str.size() && !std::isspace(static_cast<unsigned char>(str[end]))) {
                ++end;
            }

            // 添加非空白子字符串
            if (start < end) {
                result.Add(str.substr(start, end - start));
            }

            start = end;
        }
    } else {
        if (s.ByteCount() > 1) {
            Log(Warn) << "Split string view only support single char now. use first char.";
        }
        const auto ch = s[0];
        auto str = ToStdStringView();
        size_t start = 0;
        size_t end;
        // 循环查找分隔符并截取子字符串
        while ((end = str.find(ch, start)) != std::string_view::npos) {
            result.Add(str.substr(start, end - start));
            start = end + 1; // 跳过分隔符
        }

        // 添加最后一个子字符串（或整个字符串）
        result.Add(str.substr(start));
    }
    return result;
}

StringView StringView::TrimLeft(const StringView s) const {
    const auto view = s.ToStdStringView();
    auto str = ToStdStringView();
    if (s.IsEmpty()) // trim 空格
    {
        const auto left = std::ranges::find_if(str, [](auto ch) { return !std::isspace(ch); });
        if (left == str.end())
            return {};
        return str.substr(left - str.begin());
    } else {
        if (s.ByteCount() > 1) {
            Log(Warn) << "Trim string view only support single char now. use first char.";
        }
        auto ch = view[0];
        const auto left = std::ranges::find_if(str, [ch](auto c) { return c != ch; });
        if (left == str.end())
            return {};
        return str.substr(left - str.begin());
    }
}

StringView StringView::TrimRight(const StringView s) const {
    const auto view = s.ToStdStringView();
    const auto str = ToStdStringView();
    if (s.IsEmpty()) {
        size_t end = str.size();
        while (end > 0 && std::isspace(str[end - 1])) {
            --end;
        }
        return str.substr(0, end);
    } else {
        if (s.ByteCount() > 1) {
            Log(Warn) << "Trim string view only support single char now. use first char.";
        }
        const auto ch = view[0];
        size_t end = str.size();
        while (end > 0 && str[end - 1] == ch) {
            --end;
        }
        return str.substr(0, end);
    }
}

StringView StringView::Trim(const StringView s) const { return TrimLeft(s).TrimRight(s); }

StringView StringView::TrimQuotes() const { return Trim("\""); }
