/**
 * @file EString.h
 * @author Echo 
 * @Date 24-10-26
 * @brief EString.h是因为不想与string.h冲突
 */

#pragma once
#include "fmt/format.h"
#include <ostream>
#include <string>
#include <utility>

namespace core
{
class String;
class StringView
{
public:
    StringView(const char* str, const int32_t length) : str_(str), length_(length) {}
    StringView(const char* str) : str_(str), length_(static_cast<int32_t>(strlen(str))) {}
    StringView() : str_(nullptr), length_(0) {}
    StringView(const String& str);
    StringView(const std::string& str) : str_(str.c_str()), length_(static_cast<int32_t>(str.length())) {}

    [[nodiscard]] char Back() const { return str_[length_ - 1]; }

    /**
     * 移除倒数前c个字符
     * @param c
     */
    void RemoveSubfix(int c);

    [[nodiscard]] const char*       Data() const { return str_; }
    [[nodiscard]] constexpr int32_t Length() const { return length_; }
    [[nodiscard]] constexpr bool    Empty() const { return length_ == 0; }
    [[nodiscard]] bool              operator==(const StringView& o) const;
    [[nodiscard]] char              operator[](int32_t i) const { return str_[i]; }

private:
    const char* str_;
    int32_t     length_;
};

class String
{
    friend class StringView;

public:
    String() = default;
    String(std::string str) : str_(std::move(str)) {}
    String(const char* str) : str_(str) {}
    String(const char* str, int32_t length) : str_(str, length) {}
    String(const char* str_begin, const char* str_end) : str_(str_begin, str_end) {}
    String(const StringView& view);

    operator std::string() const { return str_; }
    constexpr const char* operator*() const { return str_.c_str(); }

    /**
     * 返回Unicode字符串长度
     * @return
     */
    [[nodiscard]] int32_t Size() const;

    /**
     * 返回字符数 而不是Unicode字符串长度
     * @return
     */
    [[nodiscard]] int32_t Length() const;

    [[nodiscard]] const char* Data() const { return str_.c_str(); }

    [[nodiscard]] const std::string& StdString() const { return str_; }

    String operator+(const String& str) const { return {str_ + str.str_}; }

    String& operator+=(const String& str)
    {
        str_ += str.str_;
        return *this;
    }

    auto operator<=>(const String& o) const { return str_ <=> o.str_; }

    bool operator==(const StringView& o) const;

    [[nodiscard]] bool Contains(StringView str) const;

    char operator[](int32_t i) const { return str_[i]; }

private:
    std::string str_;
};

std::ostream& operator<<(std::ostream& os, const String& str);


}   // namespace core

template<>
struct fmt::formatter<core::String>
{
    // 用于解析格式字符串（如果需要）
    constexpr auto parse(fmt::format_parse_context& ctx) -> decltype(ctx.begin())
    {
        // 在这个简单示例中不使用任何格式选项，直接返回
        return ctx.begin();
    }

    // 用于格式化输出
    template<typename FormatContext>
    auto format(const core::String& str, FormatContext& ctx) -> decltype(ctx.out())
    {
        // 将 MyType 格式化为期望的字符串形式
        return fmt::format_to(ctx.out(), "{}", *str);
    }
};

template<>
struct fmt::formatter<core::StringView>
{
    constexpr auto parse(fmt::format_parse_context& ctx) -> decltype(ctx.begin()) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const core::StringView& str, FormatContext& ctx) -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "{}", string_view(str.Data(), str.Length()));
    }
};

template<>
struct ::std::hash<core::StringView>
{
    size_t operator()(const core::StringView& str) const noexcept
    {
        return ::std::hash<std::string_view>{}(std::string_view{str.Data(), static_cast<size_t>(str.Length())});
    }
};

template<>
struct ::std::hash<core::String>
{
    size_t operator()(const core::String& str) const noexcept
    {
        return ::std::hash<std::string>{}(str.StdString());
    }
};

