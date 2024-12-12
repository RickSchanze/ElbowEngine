//
// Created by RickS on 24-12-8.
//
#pragma once
#include "Common.h"
#include "Core/Base/CoreTypeDef.h"
#include "Core/Base/TypeTraits.h"
#include "Core/CoreTypeTraits.h"
#include "Just.h"

#include <exception>

namespace core::exec
{
namespace then_detail
{
template <typename R, typename F>
struct ThenReceiver
{
    [[no_unique_address]] R receiver;
    [[no_unique_address]] F function;

    template <typename... Vs>
        requires(FunctionNoThrowInvokable<F, Vs...>() && !FunctionReturnVoid<F, Vs...>())
    friend auto TagInvoke(SetValueType, ThenReceiver&& r, Vs&&... vs) noexcept
    {
        SetValue(std::move(r.receiver), std::invoke(Move(r.function), Forward<Vs>(vs)...));
    }

    template <typename... Vs>
        requires(!FunctionNoThrowInvokable<F, Vs...>() && !FunctionReturnVoid<F, Vs...>())
    friend auto TagInvoke(SetValueType, ThenReceiver&& r, Vs&&... vs) noexcept
    {
        try
        {
            SetValue(std::move(r.receiver), std::invoke(Move(r.function), Forward<Vs>(vs)...));
        }
        catch (...)
        {
            SetError(std::move(r.receiver), std::current_exception());
        }
    }

    template <typename... Vs>
        requires(!FunctionNoThrowInvokable<F, Vs...>() && FunctionReturnVoid<F, Vs...>())
    friend auto TagInvoke(SetValueType, ThenReceiver&& r, Vs&&... vs) noexcept
    {
        try
        {
            std::invoke(Move(r.function), Forward<Vs>(vs)...);
            SetValue(std::move(r.receiver));
        }
        catch (...)
        {
            SetError(std::move(r.receiver), std::current_exception());
        }
    }

    template <typename... Vs>
        requires(FunctionNoThrowInvokable<F, Vs...>() && FunctionReturnVoid<F, Vs...>())
    friend auto TagInvoke(SetValueType, ThenReceiver&& r, Vs&&... vs) noexcept
    {
        std::invoke(Move(r.function), Forward<Vs>(vs)...);
        SetValue(std::move(r.receiver));
    }

    template <typename E>
    friend auto TagInvoke(SetErrorType, ThenReceiver&& r, E&& e) noexcept
    {
        SetError(std::move(r.receiver), Forward<E>(e));
    }

    friend auto TagInvoke(SetDoneType, ThenReceiver&& r) noexcept { SetDone(std::move(r.receiver)); }
};

template <typename Sender, typename F>
struct ThenSender
{
    using ValueTypes = typename MakeTupleType<FunctionReturnType<F>>::Type;

    Sender sender_;
    F      function_;

    template <typename SelfSender, typename Receiver>
        requires std::is_same_v<std::remove_cvref_t<SelfSender>, ThenSender>
    friend auto TagInvoke(ConnectType, SelfSender&& sender, Receiver&& receiver)
    {
        return Connect(
            Forward<SelfSender>(sender).sender_,
            ThenReceiver<Pure<Receiver>, F>{Forward<Pure<Receiver>>(receiver), Forward<SelfSender>(sender).function_}
        );
    }
};

template <typename Sender, typename F>
concept CanFunctionReceiveSender =
    CanParameterPackConvert<typename SenderTraits<std::remove_cvref_t<Sender>>::ValueTypes, FunctionArgsAsTuple<F>>::Value ||
    (std::is_same_v<typename SenderTraits<std::remove_cvref_t<Sender>>::ValueTypes, void> && FunctionTraits<F>::IsNoInput);

struct ThenType
{
    template <typename F>
    auto operator()(F&& f) const noexcept;

    template <typename SenderT, typename F>
        requires CanFunctionReceiveSender<SenderT, F>
    auto operator()(SenderT&& s, F&& f)
    {
        return ThenSender{Forward<SenderT>(s), Forward<F>(f)};
    }
};

template <typename F>
struct ThenClosure
{
    F f_;
    template <typename SenderT>
    friend auto operator|(SenderT&& s, ThenClosure&& t)
    {
        return ThenType{}(Move(s), Move(t.f_));
    }
};
template <typename F>
auto ThenType::operator()(F&& f) const noexcept
{
    return ThenClosure{Forward<F>(f)};
}
}   // namespace then_detail
inline constexpr auto Then = then_detail::ThenType{};
}   // namespace core::exec
