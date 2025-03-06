/**
 * @file Common.h
 * @author Echo 
 * @Date 24-12-4
 * @brief 
 */

#pragma once
#include "Core/Base/TagInvoke.h"
#include "Core/Base/TypeTraits.h"

#include <utility>

namespace core::exec
{
// 定制SetDone (for receiver)
struct SetDoneType
{
    template <typename Receiver>
        requires NoThrowTagInvocable<SetDoneType, Receiver>
    auto operator()(Receiver&& r) const noexcept -> TagInvokeResultType<SetDoneType, Receiver>
    {
        return tag_invoke(SetDoneType{}, std::forward<Receiver>(r));
    }
};
inline constexpr SetDoneType SetDone{};

// 定制SetError (for receiver)
struct SetErrorType
{
    template <typename Receiver, typename Error>
        requires TagInvocable<SetErrorType, Receiver, Error>
    auto operator()(Receiver&& r, Error&& e) const noexcept -> TagInvokeResultType<SetErrorType, Receiver, Error>
    {
        return tag_invoke(SetErrorType{}, std::forward<Receiver>(r), std::forward<Error>(e));
    }
};
inline constexpr SetErrorType SetError{};

// 定制SetValue (for receiver)
struct SetValueType
{
    template <typename Receiver, typename... Vs>
        requires TagInvocable<SetValueType, Receiver, Vs...>
    auto operator()(Receiver&& r, Vs&&... v) const noexcept -> TagInvokeResultType<SetValueType, Receiver, Vs...>
    {
        return tag_invoke(SetValueType{}, std::forward<Receiver>(r), std::forward<Vs>(v)...);
    }
};
inline constexpr SetValueType SetValue{};

// 定制Connect
struct ConnectType
{
    template <typename Sender, typename Receiver>
        requires TagInvocable<ConnectType, Sender, Receiver>
    auto operator()(Sender&& s, Receiver&& r) const noexcept
    {
        return tag_invoke(ConnectType{}, static_cast<Sender&&>(s), static_cast<Receiver&&>(r));
    }
};
inline constexpr ConnectType Connect{};

// 定制Start
struct StartType
{
    template <typename OperationState>
        requires TagInvocable<StartType, OperationState>
    auto operator()(OperationState&& s) const noexcept
    {
        return tag_invoke(StartType{}, std::forward<OperationState>(s));
    }
};
inline constexpr StartType Start{};

// 定制Schedule
struct ScheduleType
{
    template <typename Scheduler, typename... Args>
        requires TagInvocable<ScheduleType, Scheduler, Args...>
    auto operator()(Scheduler&& s, Args&&... args) const noexcept
    {
        return tag_invoke(ScheduleType{}, std::move(s), Forward<Args>(args)...);
    }
};
inline constexpr ScheduleType Schedule{};

template <typename SenderT>
struct SenderTraits
{
    using ValueTypes = typename Pure<SenderT>::ValueTypes;
};

template <typename Receiver>
struct ReceiverTraits
{
    using ReceiveTypes = typename Pure<Receiver>::ReceiveTypes;
};

template <typename Sender, typename Receiver>
concept CConnectable =
    std::convertible_to<typename SenderTraits<Sender>::ValueTypes, typename ReceiverTraits<Receiver>::ReceiveTypes> ||
    (std::same_as<typename SenderTraits<Sender>::ValueTypes, void> && std::same_as<typename ReceiverTraits<Receiver>::ReceiveTypes, void>);

template <typename Sender, typename Receiver>
using ConnectResultType = std::invoke_result_t<ConnectType, Sender, Receiver>;

struct Sender {};
struct Receiver {};
}   // namespace core::exec
