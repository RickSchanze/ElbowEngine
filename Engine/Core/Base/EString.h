/**
 * @file EString.h
 * @author Echo 
 * @Date 24-10-26
 * @brief EString.h是因为不想与string.h冲突
 */

#pragma once
#include <string>
#include <utility>

class String
{
    friend class StringView;

public:
    String(std::string str) : str_(std::move(str)) {}
    String(const char* str) : str_(str) {}
    String() {}
    String(const StringView& view);

    operator std::string() const { return str_; }
    const char* operator*() const { return str_.c_str(); }

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

private:
    std::string str_;
};

class StringView
{
public:
    StringView(const char* str, const int32_t length) : str_(str), length_(length) {}
    StringView(const char* str) : str_(str), length_(strlen(str)) {}
    StringView() : str_(nullptr), length_(0) {}
    StringView(const String& str) : str_(*str), length_(str.Length()) {}

    [[nodiscard]] char Back() const { return str_[length_ - 1]; }

    /**
     * 移除倒数前c个字符
     * @param c
     */
    void RemoveSubfix(int c);

    [[nodiscard]] const char* Data() const { return str_; }

    [[nodiscard]] int32_t Length() const { return length_; }

private:
    const char* str_;
    int32_t length_;
};
