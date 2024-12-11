/**
 * @file TagInvoke.h
 * @author Echo 
 * @Date 24-12-4
 * @brief 此文件提供了ElbowEngine自己的TagInvoke
 * 什么是TagInvoke? 见https://zhuanlan.zhihu.com/p/431032074
 */

#pragma once
#include <type_traits>

namespace core
{
namespace detail
{
struct TagInvokeType
{
    template <class Tag, class... Args>
    constexpr auto operator()(Tag tag, Args&&... args) const
    {
        return TagInvoke(static_cast<Tag&&>(tag), static_cast<Args&&>(args)...);
    }
};
}   // namespace detail

inline constexpr detail::TagInvokeType tag_invoke{};

template <typename T>
using TagType = std::decay_t<T>;

template <typename Tag, typename... Args>
concept TagInvocable = std::is_invocable_v<decltype(tag_invoke), Tag, Args...>;

template <typename Tag, typename... Args>
concept NoThrowTagInvocable = std::is_nothrow_invocable_v<decltype(tag_invoke), Tag, Args...>;

template <typename Tag, typename... Args>
using TagInvokeResultType = std::invoke_result_t<decltype(tag_invoke), Tag, Args...>;
}
