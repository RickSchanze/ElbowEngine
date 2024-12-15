//
// Created by RickS on 24-12-8.
//
#pragma once
#include "Concept.h"
#include "Core/Base/CoreTypeDef.h"

namespace core::exec
{
namespace just_detail
{
template <typename... Args>
struct JustSender
{
    using ValueTypes = std::tuple<Args...>;

    [[no_unique_address]] Tuple<Args...> args;

    template <typename R>
    struct Operation
    {
        [[no_unique_address]] Tuple<Args...> args;
        [[no_unique_address]] R              r;

        friend void TagInvoke(StartType, Operation& s) noexcept
        {
            try
            {
                std::apply([&s](Args&... Values) { SetValue(Move(s.r), Move(Values)...); }, s.args);
            }
            catch (...)
            {
                SetError(Forward<R>(s.r), std::current_exception());
            }
        }
    };

    template <typename Self, typename R>
        requires std::is_same_v<std::remove_cvref_t<Self>, JustSender>
    friend auto TagInvoke(ConnectType, Self&& s, R&& r)
    {
        return Operation<std::remove_cvref_t<R>>{Forward<Self>(s).args, Forward<R>(r)};
    }
};

template <typename R>
struct JustOperation
{
    [[no_unique_address]] R r;

    friend void TagInvoke(StartType, JustOperation& s) noexcept
    {
        try
        {
            SetValue(Move(s.r));
        }
        catch (...)
        {
            SetError(Move(s.r), std::current_exception());
        }
    }
};

template <>
struct JustSender<void>
{
    using ValueTypes = void;

    template <typename Self, typename R>
        requires std::is_same_v<std::remove_cvref_t<Self>, JustSender>
    friend auto TagInvoke(ConnectType, Self&& s, R&& r)
    {
        return JustOperation<std::remove_cvref_t<R>>{Forward<R>(r)};
    }
};
}   // namespace just_detail
template <typename... Args>
constexpr std::conditional_t<sizeof...(Args) == 0, just_detail::JustSender<void>, just_detail::JustSender<std::remove_cvref_t<Args>...>>
Just(Args&&... args)
{
    if constexpr (sizeof...(Args) > 0)
    {
        return just_detail::JustSender<std::remove_cvref_t<Args>...>{std::make_tuple(Forward<Args>(args)...)};
    }
    else
    {
        return just_detail::JustSender<void>{};
    }
}
}   // namespace core::exec
