//
// Created by Echo on 25-3-19.
//
#pragma once

#include "String/String.hpp"
#include "TypeAlias.hpp"

namespace internal
{
template <typename T1, typename T2>
struct InternalCanParameterPackConvert
{
    constexpr static bool value = false;
};

template <>
struct InternalCanParameterPackConvert<void, void>
{
    constexpr static bool value = true;
};

template <typename... Args1, typename... Args2>
struct InternalCanParameterPackConvert<Tuple<Args1...>, Tuple<Args2...>>
{

    constexpr static bool value = (sizeof...(Args1) == sizeof...(Args2)) && ((std::is_convertible_v<Args1, Args2>) && ...);
};

// 定义函数参数提取模板
template <typename F>
struct FunctionTraits;

// 针对普通函数类型的特化
template <typename Ret, typename... Args>
struct FunctionTraits<Ret(Args...)>
{
    using ArgsTuple = Tuple<Args...>; // 参数类型打包成 Tuple
    using ReturnType = Ret;           // 返回类型
};

// 针对函数指针的特化
template <typename Ret, typename... Args>
struct FunctionTraits<Ret (*)(Args...)> : FunctionTraits<Ret(Args...)>
{
    constexpr static bool IsNoInput = sizeof...(Args) == 0;
};

// 针对成员函数指针的特化
template <typename C, typename Ret, typename... Args>
struct FunctionTraits<Ret (C::*)(Args...)> : FunctionTraits<Ret(Args...)>
{
    constexpr static bool IsNoInput = sizeof...(Args) == 0;
};

// 针对 const 成员函数指针的特化
template <typename C, typename Ret, typename... Args>
struct FunctionTraits<Ret (C::*)(Args...) const> : FunctionTraits<Ret(Args...)>
{
    constexpr static bool IsNoInput = sizeof...(Args) == 0;
};

template <typename T>
struct IsTupleTrait : std::false_type
{
};

template <typename... Args>
struct IsTupleTrait<Tuple<Args...>> : std::true_type
{
};

// 针对 Lambda 或函数对象
template <typename T>
struct FunctionTraits : FunctionTraits<decltype(&T::operator())>
{
};

// 将一个类似std::tuple<std::tuple<int>, std::tuple<int>, std::tuple<>, std::tuple<double> 展评为 std::tuple<int, int,
// double>
template <typename T>
struct FlattenTrait
{
    using type = Tuple<T>;
};

// 处理空tuple
template <>
struct FlattenTrait<Tuple<>>
{
    using type = Tuple<>;
};

// 递归展开tuple-like类型
template <typename... Ts>
struct FlattenTrait<Tuple<Ts...>>
{
    using type = decltype(std::tuple_cat(std::declval<typename FlattenTrait<Ts>::type>()...));
};

// Any: 如果类型T是可变类型任意一种则返回true
template <typename T, typename... Ts>
struct Any : std::disjunction<std::is_same<T, Ts>...>
{
};
} // namespace internal

namespace NTraits
{
template <typename T, typename... Ts>
concept IsAnyOf = (std::is_same_v<T, Ts> || ...);

template <typename T1, typename T2>
constexpr bool CanParameterPackConvert = internal::InternalCanParameterPackConvert<T1, T2>::value;

template <typename T1, typename T2>
concept SameAs = std::same_as<T1, T2>;

template <typename T1, typename T2>
concept ConvertibleTo = std::convertible_to<T1, T2>;

template <bool T, class T2 = void>
using EnableIfType = std::enable_if_t<T, T2>;

template <typename Base, typename Derived>
constexpr bool IsBaseOf = std::is_base_of_v<Base, Derived>;

template <typename T>
constexpr bool IsReference = std::is_reference_v<T>;

template <typename T1, typename T2>
constexpr bool IsSame = std::is_same_v<T1, T2>;

template <typename T>
constexpr bool IsEnum = std::is_enum_v<T>;

template <bool Test, typename T1, typename T2>
using Conditional = std::conditional_t<Test, T1, T2>;

// 辅助别名
template <typename F>
using FunctionArgsAsTuple = typename internal::FunctionTraits<F>::ArgsTuple;

template <typename F>
using FunctionReturnType = typename internal::FunctionTraits<F>::ReturnType;

template <typename T>
constexpr bool IsTuple = internal::IsTupleTrait<T>::value;

template <typename T>
struct WrapTuple
{
    using type = std::conditional_t<IsTuple<T>, T, Tuple<T>>;
};

template <typename T>
using Flatten = typename internal::FlattenTrait<T>::type;

template <typename T>
concept CToStringAble = requires(T t) {
    { t.ToString() } -> SameAs<String>;
};
} // namespace NTraits
