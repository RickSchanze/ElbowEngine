#pragma once
#include <format>
#include <string>
#include <utility>
#include "Core/Collection/Array.hpp"
#include "Core/TypeAlias.hpp"

class String;

class StringView {
public:
    StringView(const char *str, const int32_t length) : str_(str), length_(length) {}
    StringView(const char *str) : str_(str), length_(static_cast<int32_t>(strlen(str))) {}
    StringView() : str_(nullptr), length_(0) {}
    StringView(const String &str);
    StringView(const std::string &str) : str_(str.c_str()), length_(static_cast<int32_t>(str.length())) {}
    StringView(const std::string_view &str) : str_(str.data()), length_(static_cast<int32_t>(str.length())) {}

    char Back() const { return str_[length_ - 1]; }

    /**
     * 移除倒数前c个字符
     * @param c
     */
    void RemoveSubfix(int c);

    const char *Data() const { return str_; }
    constexpr int32_t ByteCount() const { return length_; }
    constexpr bool IsEmpty() const { return length_ == 0; }
    bool IsPureSpace() const;
    StringView SubString(int32_t begin, int32_t end = -1) const;
    int32_t IndexOf(const StringView &o) const;
    int32_t LastIndexOf(const StringView &o) const;
    bool operator==(const StringView &o) const;
    bool operator==(const char *str) const;
    char operator[](const int32_t i) const { return str_[i]; }

    // 此字符串是否包含任意一个o中的字符
    bool ContainsAny(const StringView &o) const;

    bool EndsWith(const StringView &o) const;
    bool StartsWith(const StringView &o) const;
    std::wstring ToWideString() const;
    String ToString() const;

    std::string_view ToStdStringView() const { return {str_, static_cast<size_t>(length_)}; }

    String operator+(const StringView &o) const;

    Array<StringView> Split(StringView s = "") const;
    StringView TrimLeft(StringView s = "") const;
    StringView TrimRight(StringView s = "") const;
    StringView Trim(StringView s = "") const;
    StringView TrimQuotes() const;

    const char *operator*() const { return str_; }

    UInt64 GetHashCode() const { return std::hash<std::string_view>{}(ToStdStringView()); }

    auto begin() const { return ToStdStringView().begin(); }
    auto begin() { return ToStdStringView().begin(); }
    auto end() const { return ToStdStringView().end(); }
    auto end() { return ToStdStringView().end(); }

private:
    const char *str_;
    int32_t length_;
};

class String {
    friend class StringView;

public:
    String() = default;
    String(std::string str) : str_(std::move(str)) {}
    String(const char *str) : str_(str) {}
    String(const char *str, const int32_t length) : str_(str, length) {}
    String(const char *str_begin, const char *str_end) : str_(str_begin, str_end) {}
    String(const StringView &view);
    String(const String &str) = default;

    operator std::string() const { return str_; }
    constexpr const char *operator*() const { return str_.c_str(); }

    static String FromInt(const int32_t i) { return std::to_string(i); }
    static String FromWideChar(const std::wstring &wstr);

    /**
     * 返回Unicode字符串长度
     * @return
     */
    UInt64 Count() const;

    /**
     * 返回字符数 而不是Unicode字符串长度
     * @return
     */
    int32_t ByteCount() const;

    const char *Data() const { return str_.c_str(); }
    char *Data() { return str_.data(); }
    const std::string &ToStdString() const { return str_; }

    String operator+(const String &str) const { return {str_ + str.str_}; }
    String operator+(const char *str) const { return {str_ + str}; }

    String &operator+=(const String &str);

    bool operator==(const String &o) const { return str_ == o.str_; }
    bool operator<=(const String &o) const { return str_ <= o.str_; }
    bool operator>=(const String &o) const { return str_ >= o.str_; }
    bool operator<(const String &o) const { return str_ < o.str_; }
    bool operator>(const String &o) const { return str_ > o.str_; }
    bool operator!=(const String &o) const { return str_ != o.str_; }

    bool EqualsStringView(const StringView &str) const;

    bool Contains(StringView str) const;

    char &operator[](const int32_t i) { return str_[i]; }
    char operator[](const int32_t i) const { return str_[i]; }

    bool IsEmpty() const { return str_.empty(); }
    bool IsPureSpace() const;

    void Clear() { str_.clear(); }
    void Reserve(const int32_t size) { str_.reserve(size); }

    UInt8 At(const int32_t i) const { return str_.at(i); }

    StringView ToStringView() const { return {str_}; }

    std::wstring ToWideString() const;

    UInt64 GetHashCode() const;

    template<typename... Args>
    static String Format(const std::format_string<Args...> fmt, Args &&...args) {
        return std::format(fmt, std::forward<Args>(args)...);
    }

    static String FromBool(const Bool b) { return b ? "true" : "false"; }

    auto begin() const { return str_.begin(); }
    auto begin() { return str_.begin(); }
    auto end() const { return str_.end(); }
    auto end() { return str_.end(); }

private:
    std::string str_;
};

inline UInt64 Strlen(const char *str) { return strlen(str); }

// es代表elbow string
inline String operator""_es(const char *str, size_t len) {
    return {str}; // 自动调用构造函数
}

template<>
struct std::hash<String> {
    std::size_t operator()(const String &str) const noexcept { return str.GetHashCode(); }
};

template<>
struct std::hash<StringView> {
    std::size_t operator()(const StringView &str) const noexcept { return str.GetHashCode(); }
};
