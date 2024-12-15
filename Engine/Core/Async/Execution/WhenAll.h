//
// Created by Echo on 24-12-15.
//

#pragma once
#include "Common.h"
#include "Core/Base/CoreTypeDef.h"
#include "Core/Base/Ref.h"
#include "Core/Base/TypeTraits.h"
#include "Core/CoreTypeTraits.h"

namespace core::exec
{

template <int Index, typename Receiver, typename Sender>
struct WhenAllSubReceiver
{
    using ValueTypes = typename SenderTraits<Sender>::ValueTypes;

    std::shared_ptr<Pure<Receiver>> receiver;

    template <typename... Args>
        requires std::convertible_to<std::tuple<Args...>, ValueTypes>
    friend auto TagInvoke(SetValueType, WhenAllSubReceiver&& receiver, Args&&... args) noexcept
    {
        try
        {
            if constexpr (std::same_as<ValueTypes, void>)
            {
                receiver.receiver->SetValue<Index>(std::tuple{});
            }
            else
            {
                receiver.receiver->SetValue<Index>(std::tuple{Forward<Args>(args)...});
            }
        }
        catch (...)
        {
            SetError(Move(*receiver.receiver), std::current_exception());
        }
    }

    template <typename E>
    friend auto TagInvoke(SetErrorType, WhenAllSubReceiver&& r, E&& e) noexcept
    {
        SetError(std::move(*r.receiver), Forward<E>(e));
    }

    friend auto TagInvoke(SetDoneType, WhenAllSubReceiver&& r) noexcept { SetDone(std::move(*r.receiver)); }
};

template <typename Receiver, typename... Senders>
struct WhenAllReceiver
{
    typename MergeTupleNested<typename SenderTraits<Senders>::ValueTypes...>::type values;

    Receiver receiver;
    int      value_count = sizeof...(Senders);
    int      counter     = 0;

    WhenAllReceiver() = default;

    template <int Index, typename Tuple>
    void SetValue(Tuple&& tuple)
    {
        try
        {
            std::get<Index>(values) = Forward<Tuple>(tuple);
            counter++;
            if (counter >= value_count)
            {
                auto a = FlattenTuple(values);
                std::apply([this]<typename... T>(T&... args) { exec::SetValue(Move(receiver), Forward<T>(args)...); }, a);
            }
        }
        catch (...)
        {
            exec::SetError(std::move(receiver), std::current_exception());
        }
    }

    template <typename E>
    friend void TagInvoke(SetErrorType, WhenAllReceiver&& r, E&& e) noexcept
    {
        SetError(std::move(r.receiver), Forward<E>(e));
    }

    friend auto TagInvoke(SetDoneType, WhenAllReceiver&& r) noexcept { SetDone(std::move(r.receiver)); }
};

template <typename Receiver, typename... Senders>
struct WhenAllOperation
{
    std::shared_ptr<Pure<Receiver>> receiver;

    using SendersType = std::tuple<Pure<Senders>...>;

    SendersType senders;

    template <int Index = 0>
    void ApplyStart()
    {
        if constexpr (Index < sizeof...(Senders))
        {
            auto sub = WhenAllSubReceiver<Index, Receiver, std::tuple_element_t<Index, SendersType>>{receiver};
            auto op  = Connect(std::get<Index>(senders), Move(sub));
            Start(op);
            ApplyStart<Index + 1>();
        }
    }

    friend constexpr void TagInvoke(StartType, WhenAllOperation& s) noexcept { s.ApplyStart<0>(); }
};

template <typename... Senders>
struct WhenAllSender
{
    using ValueTypes = typename MergeTuples<typename SenderTraits<Senders>::ValueTypes...>::type;

    std::tuple<Pure<Senders>...> senders;

    template <typename Self, typename Receiver>
        requires(std::is_same_v<Pure<Self>, WhenAllSender> && CConnectable<WhenAllSender, Receiver>)
    friend auto TagInvoke(ConnectType, Self&& self, Receiver&& receiver)
    {
        WhenAllOperation<WhenAllReceiver<Pure<Receiver>, Senders...>, Senders...> operation;
        auto inner_receiver = std::make_shared<WhenAllReceiver<Pure<Receiver>, Senders...>>();

        inner_receiver->receiver = Move(receiver);
        operation.receiver      = inner_receiver;
        operation.senders       = self.senders;
        return operation;
    }
};

struct WhenAllType
{
    template <typename... Senders>
    auto operator()(Senders&&... senders) const noexcept
    {
        return WhenAllSender<Pure<Senders>...>{std::make_tuple(Pure<Senders>{Forward<Senders>(senders)}...)};
    }
};

inline constexpr WhenAllType WhenAll{};
}   // namespace core::exec