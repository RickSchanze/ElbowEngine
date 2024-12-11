/**
* 一些类型特征提取
*/

#pragma once
#include <type_traits>

namespace core
{
template <typename F, typename... Vs>
constexpr bool FunctionInvokable()
{
    return std::is_invocable_v<F, Vs...>;
}

template <typename F, typename... Vs>
constexpr bool FunctionReturnVoid()
{
    return std::is_void_v<std::invoke_result_t<F, Vs...>>;
}

template <typename F, typename... Vs>
constexpr bool FunctionNoThrowInvokable()
{
    return std::is_nothrow_invocable_v<F, Vs...>;
}

template <typename F, typename... Vs>
constexpr bool FunctionThrowReturnInvokable()
{
    return !std::is_nothrow_invocable_v<F, Vs...>;
}

template <typename... Args>
constexpr bool ArgTypesAllNotVoid()
{
    return (!std::is_same_v<Args, void> && ...);
}
}
