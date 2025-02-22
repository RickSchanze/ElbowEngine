/**
 * @file EString.h
 * @author Echo
 * @Date 24-10-26
 * @brief EString.h是因为不想与string.h冲突
 */

#pragma once
#include "CoreTypeDef.h"
#include "fmt/format.h"
#include <regex>
#include <string>
#include <utility>

namespace core {
class String;
class UnicodeString {
  struct Impl;

public:
  UInt64 Size() const;
  UInt32 At(UInt64 index) const;

  UnicodeString(const String &str);
  ~UnicodeString();

private:
  std::unique_ptr<Impl> impl_;
};

class StringView {
public:
  StringView(const char *str, const int32_t length) : str_(str), length_(length) {}
  StringView(const char *str) : str_(str), length_(static_cast<int32_t>(strlen(str))) {}
  StringView() : str_(nullptr), length_(0) {}
  StringView(const String &str);
  StringView(const std::string &str) : str_(str.c_str()), length_(static_cast<int32_t>(str.length())) {}
  StringView(const std::string_view &str) : str_(str.data()), length_(static_cast<int32_t>(str.length())) {}

  [[nodiscard]] char Back() const { return str_[length_ - 1]; }

  /**
   * 移除倒数前c个字符
   * @param c
   */
  void RemoveSubfix(int c);

  [[nodiscard]] const char *Data() const { return str_; }
  [[nodiscard]] constexpr int32_t Length() const { return length_; }
  [[nodiscard]] constexpr bool IsEmpty() const { return length_ == 0; }
  [[nodiscard]] bool IsPureSpace() const;
  [[nodiscard]] StringView SubString(int32_t begin, int32_t end = -1) const;
  [[nodiscard]] int32_t IndexOf(const StringView &o) const;
  [[nodiscard]] int32_t LastIndexOf(const StringView &o) const;
  [[nodiscard]] bool operator==(const StringView &o) const;
  [[nodiscard]] bool operator==(const char *str) const;
  [[nodiscard]] char operator[](int32_t i) const { return str_[i]; }

  // 此字符串是否包含任意一个o中的字符
  [[nodiscard]] bool ContainsAny(const StringView &o, bool use_utf8 = false) const;
  [[nodiscard]] bool EndsWith(const StringView &o, bool use_utf8 = false) const;
  [[nodiscard]] bool StartsWith(const StringView &o, bool use_utf8 = false) const;
  [[nodiscard]] std::wstring ToWideString() const;
  [[nodiscard]] String ToString() const;

  [[nodiscard]] std::string_view ToStdStringView() const { return {str_, static_cast<size_t>(length_)}; }

  String operator+(const StringView &o) const;

  [[nodiscard]] Array<StringView> Split(StringView s = "", bool utf8_mode = false) const;
  [[nodiscard]] StringView TrimLeft(StringView s = "", bool utf8_mode = false) const;
  [[nodiscard]] StringView TrimRight(StringView s = "", bool utf8_mode = false) const;
  [[nodiscard]] StringView Trim(StringView s = "", bool utf8_mode = false) const;
  [[nodiscard]] StringView TrimQuotes() const;

  const char *operator*() const { return str_; }

  [[nodiscard]] UInt64 GetHashCode() const { return std::hash<std::string_view>{}(ToStdStringView()); }

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
  String(const char *str, int32_t length) : str_(str, length) {}
  String(const char *str_begin, const char *str_end) : str_(str_begin, str_end) {}
  String(const StringView &view);
  String(const String &str) : str_(str.str_) {}

  operator std::string() const { return str_; }
  constexpr const char *operator*() const { return str_.c_str(); }

  static String FromInt(const int32_t i) { return std::to_string(i); }
  static String FromWideChar(const std::wstring& wstr);

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

  [[nodiscard]] const char *Data() const { return str_.c_str(); }
  [[nodiscard]] char *Data() { return str_.data(); }
  [[nodiscard]] const std::string &ToStdString() const { return str_; }

  String operator+(const String &str) const { return {str_ + str.str_}; }
  String operator+(const char *str) const { return {str_ + str}; }

  String &operator+=(const String &str);

  bool operator==(const String &o) const { return str_ == o.str_; }
  bool operator<=(const String &o) const { return str_ <= o.str_; }
  bool operator>=(const String &o) const { return str_ >= o.str_; }
  bool operator<(const String &o) const { return str_ < o.str_; }
  bool operator>(const String &o) const { return str_ > o.str_; }
  bool operator!=(const String &o) const { return str_ != o.str_; }

  [[nodiscard]] bool EqualsStringView(const StringView &str) const;

  [[nodiscard]] bool Contains(StringView str) const;

  char &operator[](int32_t i) { return str_[i]; }
  char operator[](int32_t i) const { return str_[i]; }

  [[nodiscard]] bool IsEmpty() const { return str_.empty(); }
  [[nodiscard]] bool IsPureSpace() const;

  void Clear() { str_.clear(); }
  void Reserve(int32_t size) { str_.reserve(size); }

  [[nodiscard]] UInt8 At(int32_t i) const { return str_.at(i); }
  [[nodiscard]] UInt32 AtUnicode(int32_t i) const;

  template <typename... Args> static String Format(fmt::format_string<Args...> format, Args &&...args) {
    return fmt::format(format, std::forward<Args>(args)...);
  }

  [[nodiscard]] core::StringView ToStringView() const { return {str_}; }

  UInt64 GetHashCode() const;

  /**
   * 返回一个unicode string
   * 会进行复制 用于字体, 一般你不需要调用
   * @return
   */
  UnicodeString ToUnicodeString() const;

  std::wstring ToWideString() const;

private:
  std::string str_;
};

std::ostream &operator<<(std::ostream &os, const String &str);

bool Equals(const core::String &a, const core::StringView &b);

} // namespace core

template <> struct fmt::formatter<core::String> {
  // 用于解析格式字符串（如果需要）
  constexpr auto parse(fmt::format_parse_context &ctx) -> decltype(ctx.begin()) {
    // 在这个简单示例中不使用任何格式选项，直接返回
    return ctx.begin();
  }

  // 用于格式化输出
  template <typename FormatContext> auto format(const core::String &str, FormatContext &ctx) -> decltype(ctx.out()) {
    // 将 MyType 格式化为期望的字符串形式
    return fmt::format_to(ctx.out(), "{}", *str);
  }
};

template <> struct fmt::formatter<core::StringView> {
  constexpr auto parse(fmt::format_parse_context &ctx) -> decltype(ctx.begin()) { return ctx.begin(); }
  template <typename FormatContext>
  auto format(const core::StringView &str, FormatContext &ctx) -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "{}", string_view(str.Data(), str.Length()));
  }
};

template <> struct ::std::hash<core::StringView> {
  size_t operator()(const core::StringView &str) const noexcept {
    return ::std::hash<std::string_view>{}(std::string_view{str.Data(), static_cast<size_t>(str.Length())});
  }
};

template <> struct ::std::hash<core::String> {
  size_t operator()(const core::String &str) const noexcept { return ::std::hash<std::string>{}(str.ToStdString()); }
};
