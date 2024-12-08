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
                std::apply([&s](Args&... Values) { SetValue(s.r, Move(Values)...); }, s.args);
            }
            catch (...)
            {
                SetError(Forward<R>(s.r), std::current_exception());
            }
        }
    };

    template <typename Self, typename R>
        requires std::is_same_v<std::remove_cvref_t<Self>, JustSender> && CReceiverOf<R, Args...>
    friend auto TagInvoke(ConnectType, Self&& s, R&& r)
    {
        return Operation<std::remove_cvref_t<R>>{Forward<Self>(s).args, Forward<R>(r)};
    }
};
}   // namespace just_detail
template <typename... Args>
constexpr just_detail::JustSender<std::remove_cvref_t<Args>...> Just(Args&&... args)
{
    return just_detail::JustSender<std::remove_cvref_t<Args>...>{Forward<Args>(args)...};
}
}   // namespace core::exec
