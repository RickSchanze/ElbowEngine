//
// Created by Echo on 24-12-11.
//

#pragma once
#include "Common.h"
#include "Core/Base/TypeTraits.h"
#include "Core/Log/CoreLogCategory.h"
#include "Core/Log/Logger.h"

namespace core::exec
{
template <typename... Args>
struct NullReceiver;

template <typename... Args>
struct NullReceiver
{
    static_assert(ArgTypesAllNotVoid<Args...>::Value, "Arg types can not bo void");

    using ReceiveTypes = std::tuple<Args...>;

    template <typename Receiver>
        requires std::is_same_v<Pure<Receiver>, NullReceiver>
    friend auto TagInvoke(SetValueType, Receiver&& r, Args&&... vs) noexcept
    {
        (LOGGER.Info(logcat::Test, "NullReceiver receive value: {}", std::forward<Args>(vs)), ...);
    }

    template <typename Receiver, typename E>
        requires std::is_same_v<std::remove_cvref_t<Receiver>, NullReceiver>
    friend auto TagInvoke(SetErrorType, Receiver&& r, E&& e) noexcept
    {
        LOGGER.Info(logcat::Test, "NullReceiver receive error");
    }

    template <typename Receiver>
        requires std::is_same_v<std::remove_cvref_t<Receiver>, NullReceiver>
    friend auto TagInvoke(SetDoneType, Receiver&& r) noexcept
    {
        LOGGER.Info(logcat::Test, "NullReceiver receive done");
    }
};

// void特化
template <>
struct NullReceiver<void>
{
    template <typename Receiver>
        requires std::is_same_v<std::remove_cvref_t<Receiver>, NullReceiver>
    friend auto TagInvoke(SetValueType, Receiver&& r) noexcept
    {
        LOGGER.Info(logcat::Test, "NullReceiver receive value: void");
    }

    template <typename Receiver, typename E>
        requires std::is_same_v<std::remove_cvref_t<Receiver>, NullReceiver>
    friend auto TagInvoke(SetErrorType, Receiver&& r, E&& e) noexcept
    {
        LOGGER.Info(logcat::Test, "NullReceiver receive error");
    }

    template <typename Receiver>
        requires std::is_same_v<std::remove_cvref_t<Receiver>, NullReceiver>
    friend auto TagInvoke(SetDoneType, Receiver&& r) noexcept
    {
        LOGGER.Info(logcat::Test, "NullReceiver receive done");
    }
};


}   // namespace core::exec
