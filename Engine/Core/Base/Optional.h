#pragma once

#include "Exception.h"

#include <type_traits>   // for std::aligned_storage_t, std::is_trivially_destructible
#include <utility>       // for std::move and std::forward

namespace core
{

class OptionalValueException : public Exception
{
public:
    OptionalValueException() : Exception("Optional没有值") {}
};

template <typename T>
class Optional
{
private:
    // 使用 std::aligned_storage_t 分配对齐的未构造内存用于存储值
    std::aligned_storage_t<sizeof(T), alignof(T)> storage;
    T*                                            value_ptr = nullptr;   // 指针是否为空表示是否有效

    // 将 storage 转换为 T 类型的指针
    T* AsValueInternal() { return reinterpret_cast<T*>(&storage); }

    const T* AsValueInternal() const { return reinterpret_cast<const T*>(&storage); }

    T* AsValue()
    {
        if (!value_ptr)
        {
            throw OptionalValueException();
        }
        return value_ptr;
    }

    const T* AsValue() const
    {
        if (!value_ptr) throw OptionalValueException();
        return value_ptr;
    }

public:
    // 默认构造函数，不包含值
    Optional() noexcept : value_ptr(nullptr) {}

    // 构造函数，构造一个值
    Optional(const T& val)
    {
        new (&storage) T(val);
        value_ptr = AsValueInternal();
    }

    // 移动构造函数
    Optional(T&& val) noexcept(std::is_nothrow_move_constructible_v<T>)
    {
        new (&storage) T(std::move(val));
        value_ptr = AsValueInternal();
    }

    // 析构函数
    ~Optional() { Reset(); }

    // 拷贝构造函数
    Optional(const Optional& other)
    {
        if (other.HasValue())
        {
            new (&storage) T(*other.AsValueInternal());
            value_ptr = AsValueInternal();
        }
        else
        {
            value_ptr = nullptr;
        }
    }

    // 移动构造函数
    Optional(Optional&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
    {
        if (other.HasValue())
        {
            new (&storage) T(std::move(*other.AsValueInternal()));
            value_ptr = AsValueInternal();
        }
        else
        {
            value_ptr = nullptr;
        }
    }

    // 拷贝赋值运算符
    Optional& operator=(const Optional& other)
    {
        if (this != &other)
        {
            Reset();
            if (other.HasValue())
            {
                new (&storage) T(*other.AsValueInternal());
                value_ptr = AsValueInternal();
            }
        }
        return *this;
    }

    // 移动赋值运算符
    Optional& operator=(Optional&& other) noexcept(std::is_nothrow_move_assignable_v<T>)
    {
        if (this != &other)
        {
            Reset();
            if (other.HasValue())
            {
                new (&storage) T(std::move(*other.AsValueInternal()));
                value_ptr = AsValueInternal();
            }
        }
        return *this;
    }

    // 检查是否包含值
    [[nodiscard]] bool HasValue() const noexcept { return value_ptr != nullptr; }

    operator bool() const noexcept { return HasValue(); }

    // 获取值
    T& operator*() { return *AsValue(); }

    const T& operator*() const { return *AsValue(); }

    T* operator->() { return AsValue(); }

    const T* operator->() const { return AsValue(); }

    // 重置
    void Reset()
    {
        if (value_ptr)
        {
            AsValueInternal()->~T();   // 显式调用析构函数
            value_ptr = nullptr;
        }
    }

    // 赋值新值
    template <typename... Args>
    void Emplace(Args&&... args)
    {
        Reset();
        new (&storage) T(std::forward<Args>(args)...);
        value_ptr = AsValueInternal();
    }
};

template <typename T>
Optional<T> MakeOptional(const T& val) { return Optional<T>(val); }
}