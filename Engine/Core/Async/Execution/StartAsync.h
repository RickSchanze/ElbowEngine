//
// Created by Echo on 24-12-15.
// TODO: shared_ptr的自定义内存分配
//

#pragma once
#include "Common.h"
#include "Core/Async/ThreadUtils.h"
#include "Core/Base/CoreTypeDef.h"
#include "Core/Base/Optional.h"
#include "Core/Base/TypeTraits.h"
#include "Core/Log/Logger.h"
#include "Resource/Logcat.h"

#include <condition_variable>

namespace core::exec
{

struct AsyncResultBase
{
    std::atomic<bool>  ready = false;
    std::exception_ptr exception{};

    void SetException(const std::exception_ptr& _exception) { exception = _exception; }

    std::exception_ptr GetException() { return exception; }

    void SetDone() { ready.store(true); }

    bool IsDone() const { return ready.load(); }

    virtual bool IsCanceled() const { return false; }

    template <typename... Args>
    Optional<std::tuple<Args...>> GetAsyncResult();
};


template <typename T>
struct AsyncResult : public AsyncResultBase
{
    Optional<T> data{};

    bool IsCanceled() const override { return IsDone() && !data; }

    void SetData(const T& _data)
    {
        data = _data;
        ready.store(true);
    }

    Optional<T> GetValue()
    {
        if (exception)
        {
            if (IsDone())
            {
                return {};
            }
            std::rethrow_exception(exception);
        }
        return data;
    }
};

template <typename... Args>
Optional<std::tuple<Args...>> AsyncResultBase::GetAsyncResult()
{
    return static_cast<AsyncResult<std::tuple<Args...>>*>(this)->GetValue();
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

#define NULL_ASYNC_RESULT_HANDLE nullptr

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
    AsyncResultHandle operator()(AsyncResultHandle re) const
    {
        if (!re) return re;
        while (!re->IsDone())
        {
            ThreadUtils::YieldThread();
        }
        return re;
    }
};

inline constexpr SyncWaitType SyncWait{};
}   // namespace core::exec
