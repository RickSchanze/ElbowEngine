//
// Created by Echo on 2025/3/20.
//

#pragma once
#include <chrono>
#include <cstdint>
#include <functional>
#include <mutex>
#include <span>

#undef max
#undef min

template<typename T>
using Function = std::function<T>;

template<typename T>
using Pure = std::remove_cvref_t<T>;

// 将左值/右值无条件转换为右值引用
template<typename T>
std::remove_reference_t<T> &&Move(T &&arg) noexcept {
    return static_cast<std::remove_reference_t<T> &&>(arg);
}

// 左值版本：保留左值属性
template<typename T>
T &&Forward(std::remove_reference_t<T> &arg) noexcept {
    return static_cast<T &&>(arg);
}

// 右值版本：转换为右值
template<typename T>
T &&Forward(std::remove_reference_t<T> &&arg) noexcept {
    return static_cast<T &&>(arg);
}

template<typename... Args>
using Tuple = std::tuple<Args...>;

template<typename T1, typename T2>
void Swap(T1 &&lhs, T2 &&rhs) {
    std::swap(Forward<T1>(lhs), Forward<T2>(rhs));
}

template<typename T1, typename T2>
using Pair = std::pair<T1, T2>;

using ExceptionPtr = std::exception_ptr;

template<typename... Args>
auto MakeTuple(Args &&...args) {
    return std::make_tuple(std::forward<Args>(args)...);
}

template<typename Func, typename Tup>
auto Apply(Func &&func, Tup &&tup) {
    return std::apply(std::forward<Func>(func), std::forward<Tup>(tup));
}

template<size_t I, typename... Args>
auto Get(Tuple<Args...> &&tup) {
    return std::get<I>(std::forward<Tuple<Args...>>(tup));
}

template<size_t I, typename... Args>
using TupleElementType = std::tuple_element_t<I, Tuple<Args...>>;

using Mutex = std::mutex;

template<typename T>
using AutoLock = std::lock_guard<T>;

using Int8 = int8_t;
using Int16 = int16_t;
using Int32 = int32_t;
using Int64 = int64_t;
using UInt8 = uint8_t;
using UInt16 = uint16_t;
using UInt32 = uint32_t;
using UInt64 = uint64_t;
using Float = float;
using Double = double;
using Bool = bool;

template<typename T>
constexpr T NumberMax() {
    return std::numeric_limits<T>::max();
}

template<typename T>
constexpr T NumberMin() {
    return std::numeric_limits<T>::min();
}

using MilliSeconds = std::chrono::duration<float, std::milli>;
using Seconds = std::chrono::duration<float>;

template<typename T>
using Atomic = std::atomic<T>;

template<typename T, size_t Extent = std::dynamic_extent>
using Span = std::span<T, Extent>;

typedef std::chrono::steady_clock::time_point TimePoint;

inline TimePoint Now() { return std::chrono::steady_clock::now(); }

template<class To, class Rep, class Period>
To CastDuration(const std::chrono::duration<Rep, Period> &dur) {
    return std::chrono::duration_cast<To>(dur);
}

template<typename Rep, typename Period>
using Duration = std::chrono::duration<Rep, Period>;
