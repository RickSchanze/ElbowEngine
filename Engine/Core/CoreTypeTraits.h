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
template <typename T>
concept IsNumeric = std::is_arithmetic_v<T>;

// 是个布尔类型
template <typename T>
concept IsBoolean = std::is_same_v<T, bool>;

template <typename T>
concept Callable = std::is_invocable_v<T>;

template <typename T1, typename T2>
struct CanParameterPackConvert
{
    constexpr static bool Value = false;
};

template <typename... Args1, typename... Args2>
struct CanParameterPackConvert<std::tuple<Args1...>, std::tuple<Args2...>>
{
    constexpr static bool Value = (sizeof...(Args1) == sizeof...(Args2)) && ((std::is_convertible_v<Args1, Args2>) && ...);
};

template <typename T, typename... Types>
concept OneOf = (std::is_same_v<T, Types> || ...);


template <typename T>
concept ArrayLikeIterable = requires(T t) {
    t.begin();
    t.end();
    t.cbegin();
    t.cend();
    typename T::value_type;
} && !requires(T t) { typename T::key_type; };

template <typename T>
concept MapLikeIterable = requires(T t) {
    t.begin();
    t.end();
    t.cbegin();
    t.cend();
    typename T::key_type;
    typename T::value_type;
    typename T::mapped_type;
};

template <typename T>
concept IsStaticArray = !requires(T a, typename T::const_iterator it, const typename T::value_type& v) {
    { a.insert(it, v) };   // 检查是否存在名称为insert的方法
} && ArrayLikeIterable<T>;

template <typename T>
struct IsTypeHasTemplate : std::false_type
{
};

template <template <typename...> typename Template, typename... Args>
struct IsTypeHasTemplate<Template<Args...>> : std::true_type
{
};

// 定义函数参数提取模板
template <typename F>
struct FunctionTraits;

// 针对普通函数类型的特化
template <typename Ret, typename... Args>
struct FunctionTraits<Ret(Args...)>
{
    using ArgsTuple  = std::tuple<Args...>;   // 参数类型打包成 Tuple
    using ReturnType = Ret;                   // 返回类型
};

// 针对函数指针的特化
template <typename Ret, typename... Args>
struct FunctionTraits<Ret (*)(Args...)> : FunctionTraits<Ret(Args...)>
{
};

// 针对成员函数指针的特化
template <typename C, typename Ret, typename... Args>
struct FunctionTraits<Ret (C::*)(Args...)> : FunctionTraits<Ret(Args...)>
{
};

// 针对 const 成员函数指针的特化
template <typename C, typename Ret, typename... Args>
struct FunctionTraits<Ret (C::*)(Args...) const> : FunctionTraits<Ret(Args...)>
{
};

// 针对 Lambda 或函数对象
template <typename T>
struct FunctionTraits : FunctionTraits<decltype(&T::operator())>
{
};

// 辅助别名
template <typename F>
using FunctionArgsAsTuple = typename FunctionTraits<F>::ArgsTuple;

template <typename F>
using FunctionReturnType = typename FunctionTraits<F>::ReturnType;
