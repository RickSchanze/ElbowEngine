//
// Created by Echo on 24-12-15.
//

#pragma once
#include "Common.h"
#include "Core/Base/CoreTypeDef.h"
#include "Core/Base/TypeTraits.h"

namespace core::exec
{

template <typename T>
struct AsyncDataHolder
{
    std::exception_ptr exception{};

    T    data{};
    bool done = false;

    void SetData(const T& _data)
    {
        data = _data;
        done = true;
    }

    void SetException(const std::exception_ptr& _exception) { exception = _exception; }

    void SetDone() { done = true; }

    bool IsDone() const { return done; }

    std::optional<T> GetValue()
    {
        if (exception)
        {
            if (done)
            {
                return {};
            }
            std::rethrow_exception(exception);
        }
        return data;
    }

    std::exception_ptr GetException() { return exception; }
};

template <typename... Args>
struct AsyncReceiver
{
    using ReceiveTypes = std::tuple<Args...>;

    std::shared_ptr<AsyncDataHolder<ReceiveTypes>> data;

    template <typename Receiver>
        requires std::is_same_v<Pure<Receiver>, AsyncReceiver>
    friend auto TagInvoke(SetValueType, Receiver&& r, Args&&... vs) noexcept
    {
        r.data->SetData(std::make_tuple(std::forward<Args>(vs)...));
    }

    template <typename Receiver, typename E>
        requires std::is_same_v<std::remove_cvref_t<Receiver>, AsyncReceiver>
    friend auto TagInvoke(SetErrorType, Receiver&& r, E&& e) noexcept
    {
        r.data->SetException(e);
    }

    template <typename Receiver>
        requires std::is_same_v<std::remove_cvref_t<Receiver>, AsyncReceiver>
    friend auto TagInvoke(SetDoneType, Receiver&& r) noexcept
    {
        r.data->SetDone();
    }
};

template <typename... Args>
struct AsyncWrapper;

template <typename... Args>
struct AsyncWrapper<std::tuple<Args...>>
{
    using ReceiverType = AsyncReceiver<Args...>;
};

struct StartAsyncType
{
    template <typename Sender>
    auto operator()(Sender sender) const
    {
        using ValueTypes = typename SenderTraits<Sender>::ValueTypes;

        auto holder = std::make_shared<AsyncDataHolder<ValueTypes>>();
        typename AsyncWrapper<ValueTypes>::ReceiverType receiver;
        receiver.data = holder;
        auto op       = Connect(std::move(sender), receiver);
        Start(op);
        return holder;
    }
};

inline constexpr StartAsyncType StartAsync{};

struct SyncWaitType
{
    template <typename T>
    auto operator()(const std::shared_ptr<AsyncDataHolder<T>>& re) const
    {
        while (!re->IsDone())
        {
        }
        return re->GetValue();
    }
};

inline constexpr SyncWaitType SyncWait{};
}   // namespace core::exec
