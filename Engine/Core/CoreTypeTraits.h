/**
 * @file CoreTypeTraits.h
 * @author Echo 
 * @Date 2024/4/11
 * @brief 
 */

#pragma once

#include <tuple>
#include <type_traits>
// 数字类型
template<typename T>
concept IsNumeric = std::is_arithmetic_v<T>;

// 是个布尔类型
template<typename T>
concept IsBoolean = std::is_same_v<T, bool>;

template<typename T>
concept Callable = std::is_invocable_v<T>;

template<typename T1, typename T2>
struct CanParameterPackConvert;

template<typename... Args1, typename... Args2>
struct CanParameterPackConvert<std::tuple<Args1...>, std::tuple<Args2...>>
{
    constexpr static bool Value = (sizeof...(Args1) == sizeof...(Args2)) && ((std::is_convertible_v<Args1, Args2>)&&...);
};

template<typename T, typename... Types>
concept OneOf = (std::is_same_v<T, Types> || ...);


template<typename T>
concept ArrayLikeIterable = requires(T t) {
    t.begin();
    t.end();
    t.cbegin();
    t.cend();
    typename T::value_type;
} && !requires(T t) { typename T::key_type; };

template<typename T>
concept MapLikeIterable = requires(T t) {
    t.begin();
    t.end();
    t.cbegin();
    t.cend();
    T::key_type;
    T::value_type;
};

