/**
 * @file Common.h
 * @author Echo 
 * @Date 24-12-4
 * @brief 
 */

#pragma once
#include "Core/Base/TagInvoke.h"
#include "Core/Log/CoreLogCategory.h"
#include "Core/Log/Logger.h"
#include "Resource/Logcat.h"


#include <exception>
#include <utility>

namespace core::execution
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

template <class T, class E = std::exception_ptr>
concept CReceiver = requires(std::remove_cvref_t<T>&& t, E&& e) {
    std::move_constructible<std::remove_cvref_t<T>>&& std::constructible_from<std::remove_cvref_t<T>, T>;
    { SetDone(std::move(t)) } noexcept;
    { SetError(std::move(t), std::forward<E>(e)) } noexcept;
};

template <class T, class... Args>
concept CReceiverOfConcept = CReceiver<T> && requires(std::remove_cvref_t<T>&& t, Args&&... args) {
    { SetValue(std::move(t), std::forward<Args>(args)...) } noexcept;
};

// 定制Connect
struct ConnectType
{
    template <typename Sender, CReceiver Receiver>
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
}   // namespace core::execution
