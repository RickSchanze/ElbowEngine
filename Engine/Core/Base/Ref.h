/**
 * @file Ref.h
 * @author Echo 
 * @Date 24-10-27
 * @brief 
 */

#pragma once
#include "fmt/format.h"
#include <type_traits>

namespace core
{
template <typename T>
struct Ref
{
    static_assert(!std::is_reference_v<T>, "Ref cannot apply on reference type");
    using value_type = T;

    Ref(T& ref) : ptr_(&ref) {}
    Ref(const Ref& other) : ptr_(other.ptr_) {}
    Ref(Ref&& other) : ptr_(other.ptr_) {}

    T* operator->() { return ptr_; }

    T& operator*() { return *ptr_; }

    T* operator->() const { return ptr_; }

    T& operator*() const { return *ptr_; }

    T*       GetPtr() { return ptr_; }
    const T* GetPtr() const { return ptr_; }

    bool operator==(const Ref& other) const { return ptr_ == other.ptr_; }
    bool operator==(const T& other) const { return *ptr_ == other; }

    Ref& operator=(const T& other)
    {
        *ptr_ = other;
        return *this;
    }

    Ref& operator=(T&& other)
    {
        *ptr_ = Move(other);
        return *this;
    }

    operator T&() { return *ptr_; }

private:
    T* ptr_ = nullptr;
};

template <typename T>
Ref<T> MakeRef(T& ref)
{
    return Ref<T>(ref);
}

template <typename T>
struct IsRef : std::false_type
{
};

template <typename T>
struct IsRef<Ref<T>> : std::true_type
{
};

template <typename T>
constexpr bool IsRef_V = IsRef<T>::value;

}   // namespace core

template <typename T>
struct fmt::formatter<core::Ref<T>>
{
    constexpr auto parse(fmt::format_parse_context& ctx) -> decltype(ctx.begin()) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const core::Ref<T>& ref, FormatContext& ctx) -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "{}", *ref);
    }
};
