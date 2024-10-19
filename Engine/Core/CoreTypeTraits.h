/**
 * @file CoreTypeTraits.h
 * @author Echo 
 * @Date 2024/4/11
 * @brief 
 */

#pragma once

#include <type_traits>
// 数字类型
template<typename T>
concept IsNumeric = std::is_arithmetic_v<T>;

// 是个布尔类型
template<typename T>
concept IsBoolean = std::is_same_v<T, bool>;

template<typename T>
concept Callable = std::is_invocable_v<T>;

// 函数类型萃取
template<typename>
struct FunctionTrait;

// 针对函数类型的特化
template<typename Ret, typename... Args>
struct FunctionTrait<Ret(Args...)>
{
    using ReturnType    = Ret;
    using ArgumentTypes = std::tuple<Args...>;
};

// 针对函数指针的特化
template<typename Ret, typename... Args>
struct FunctionTrait<Ret (*)(Args...)> : FunctionTrait<Ret(Args...)>
{
};

// 针对成员函数指针的特化
template<typename Ret, typename Class, typename... Args>
struct FunctionTrait<Ret (Class::*)(Args...)> : FunctionTrait<Ret(Args...)>
{
};

// 针对const成员函数指针的特化
template<typename Ret, typename Class, typename... Args>
struct FunctionTrait<Ret (Class::*)(Args...) const> : FunctionTrait<Ret(Args...)>
{
};

// 函数类型萃取(返回值)
template<typename T>
using ReturnTypeOf = typename FunctionTrait<T>::ReturnType;

// 函数类型萃取(参数类型)
template<typename T>
using ArgumentTypesOf = typename FunctionTrait<T>::ArgumentTypes;

// 萃取第i个参数类型
template<size_t i, typename T>
using ArgumentTypeOf =  std::tuple_element_t<i, ArgumentTypesOf<T>>;
