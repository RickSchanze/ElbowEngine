//
// Created by Echo on 24-12-15.
// TODO: shared_ptr的自定义内存分配
//

#pragma once
#include "Common.h"
#include "Core/Base/CoreTypeDef.h"
#include "Core/Base/Optional.h"
#include "Core/Base/TypeTraits.h"

namespace core::exec
{

struct AsyncResultBase
{
    bool               done = false;
    std::exception_ptr exception{};

    void SetException(const std::exception_ptr& _exception) { exception = _exception; }

    std::exception_ptr GetException() { return exception; }

    void SetDone() { done = true; }

    bool IsDone() const { return done; }

    virtual bool IsCanceled() const { return false; }
};

template <typename T>
struct AsyncResult : public AsyncResultBase
{
    Optional<T> data{};

    bool IsCanceled() const override { return IsDone() && !data; }

    void SetData(const T& _data)
    {
        data = _data;
        done = true;
    }

    Optional<T> GetValue()
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
};

template <typename... Args>
Optional<std::tuple<Args...>> GetAsyncResult(const AsyncResultBase& result)
{
    return static_cast<AsyncResult<std::tuple<Args...>>*>(&result)->GetValue();
}

template <typename... Args>
struct AsyncReceiver
{
    using ReceiveTypes = std::tuple<Args...>;

    std::shared_ptr<AsyncResult<ReceiveTypes>> data;

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

template <>
struct AsyncWrapper<void>
{
    using ReceiverType = AsyncReceiver<>;
};

using AsyncResultHandle = SharedPtr<AsyncResultBase>;

struct StartAsyncType
{
    template <typename Sender>
    AsyncResultHandle operator()(Sender sender) const
    {
        using ValueTypes = typename SenderTraits<Sender>::ValueTypes;
        typename AsyncWrapper<ValueTypes>::ReceiverType receiver;
        if constexpr (std::is_same_v<ValueTypes, void>)
        {
            auto              holder = std::make_shared<AsyncResult<std::tuple<>>>();
            AsyncResultHandle rtn    = holder;
            receiver.data            = holder;
            auto op                  = Connect(std::move(sender), receiver);
            Start(op);
            return rtn;
        }
        else
        {
            auto              holder = std::make_shared<AsyncResult<ValueTypes>>();
            AsyncResultHandle rtn    = holder;
            receiver.data            = holder;
            auto op                  = Connect(std::move(sender), receiver);
            Start(op);
            return rtn;
        }
    }
};

inline constexpr StartAsyncType StartAsync{};

struct SyncWaitType
{
    void operator()(const AsyncResultHandle& re) const
    {
        if (!re) return;
        while (!re->IsDone())
        {
        }
    }
};

inline constexpr SyncWaitType SyncWait{};
}   // namespace core::exec
