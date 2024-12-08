/**
 * @file Common.h
 * @author Echo 
 * @Date 24-12-4
 * @brief 
 */

#pragma once
#include "Core/Base/TagInvoke.h"

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
        return TagInvoke(SetDoneType{}, std::forward<Receiver>(r));
    }
};
inline constexpr SetDoneType SetDone{};

// 定制SetError (for receiver)
struct SetErrorType
{
    template <typename Receiver, typename Error>
        requires NoThrowTagInvocable<SetErrorType, Receiver, Error>
    auto operator()(Receiver&& r, Error&& e) const noexcept -> TagInvokeResultType<SetErrorType, Receiver, Error>
    {
        return TagInvoke(SetErrorType{}, std::forward<Receiver>(r), std::forward<Error>(e));
    }
};
inline constexpr SetErrorType SetError{};

// 定制SetValue (for receiver)
struct SetValueType
{
    template <typename Receiver, typename Value>
        requires NoThrowTagInvocable<SetValueType, Receiver, Value>
    auto operator()(Receiver&& r, Value&& v) const noexcept -> TagInvokeResultType<SetValueType, Receiver, Value>
    {
        return TagInvoke(SetValueType{}, std::forward<Receiver>(r), std::forward<Value>(v));
    }
};
inline constexpr SetValueType SetValue{};

// 定制Connect
struct ConnectType
{
    template <typename Sender, typename Receiver>
        requires TagInvocable<ConnectType, Sender, Receiver>
    auto operator()(Sender&& s, Receiver&& r) const noexcept -> TagInvokeResultType<ConnectType, Sender, Receiver>
    {
        return TagInvoke(ConnectType{}, std::forward<Sender>(s), std::forward<Receiver>(r));
    }
};
inline constexpr ConnectType Connect{};

// 定制Start
struct StartType
{
    template <typename OperationState>
        requires TagInvocable<StartType, OperationState>
    auto operator()(OperationState&& s) const noexcept -> TagInvokeResultType<StartType, OperationState>
    {
        return TagInvoke(StartType{}, std::forward<OperationState>(s));
    }
};
inline constexpr StartType Start{};

template <typename Sender>
struct SenderTraits
{
    using ValueTypes = typename Sender::ValueTypes;
};
}   // namespace core::exec
