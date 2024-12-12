//
// Created by Echo on 24-12-11.
//

#pragma once
#include "Common.h"
#include "Core/Base/CoreTypeDef.h"
#include "Core/CoreTypeTraits.h"

#include <exception>

namespace core::exec
{
namespace repeat_details
{
template <typename NextReceiver>
struct RepeatReceiver
{
    NextReceiver next_receiver;

    template <typename R>
        requires std::is_same_v<std::remove_cvref_t<R>, RepeatReceiver>
    friend auto TagInvoke(SetValueType, R&& self)
    {
        // 啥都不敢, 就等待重复
    }

    template <typename R, typename E>
        requires std::is_same_v<std::remove_cvref_t<R>, RepeatReceiver>
    friend auto TagInvoke(SetErrorType, R&& Self, E&& error)
    {
        SetError(std::move(Self.next_receiver), std::forward<E>(error));
    }

    template <typename R>
        requires std::is_same_v<std::remove_cvref_t<R>, RepeatReceiver>
    friend auto TagInvoke(SetDoneType, R&& Self)
    {
        SetDone(std::move(Self.next_receiver));
    }
};

template <typename PreviousSender, typename Receiver>
struct RepeatOperation
{
    Receiver       receiver;
    PreviousSender previous_sender;
    int            repeat_count;

    friend void TagInvoke(StartType, RepeatOperation& operation)
    {
        if (operation.repeat_count > 0)
        {
            auto previous_op = Connect(operation.previous_sender, operation.receiver);
            for (auto i = 0; i < operation.repeat_count; ++i)
            {
                Start(previous_op);
            }
            SetValue(std::move(operation.receiver.next_receiver));
        }
    }
};

template <typename PreviousSender>
struct RepeatSender
{
    using ValueTypes = void;

    PreviousSender previous_sender;
    int            repeat_count = 0;

    template <typename SelfSender, typename Receiver>
        requires std::is_same_v<std::remove_cvref_t<SelfSender>, RepeatSender>
    friend auto TagInvoke(ConnectType, SelfSender&& self, Receiver&& receiver)
    {
        RepeatOperation<Pure<PreviousSender>, RepeatReceiver<Pure<Receiver>>> operation;
        operation.receiver        = RepeatReceiver<Pure<Receiver>>(receiver);
        operation.previous_sender = self.previous_sender;
        operation.repeat_count    = self.repeat_count;
        return operation;
    }
};

struct RepeatType
{
    auto operator()(int cnt) const noexcept;

    template <typename Sender>
    auto operator()(Sender&& sender, int cnt)
    {
        return RepeatSender<Sender>{std::forward<Sender>(sender), cnt};
    }
};

struct RepeatClosure
{
    int repeat_count = 0;

    template <typename SenderT>
        requires std::is_same_v<typename SenderTraits<std::remove_cvref_t<SenderT>>::ValueTypes, void>
    friend auto operator|(SenderT&& sender, RepeatClosure&& closure)
    {
        return RepeatType{}(std::forward<std::remove_cvref_t<SenderT>>(sender), closure.repeat_count);
    }
};

inline auto RepeatType::operator()(const int cnt) const noexcept
{
    return RepeatClosure{cnt};
}
}   // namespace repeat_details
inline constexpr auto Repeat = repeat_details::RepeatType{};
}   // namespace core::exec