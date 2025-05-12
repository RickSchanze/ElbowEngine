//
// Created by Echo on 2025/3/22.
//

#pragma once
#include "Common.hpp"
#include "Core/Async/Future.hpp"
#include "Core/Async/Promise.hpp"

namespace NExec
{
template <typename... Args>
struct FutureReceiver : Receiver
{
    FutureReceiver() = default;
    FutureReceiver(FutureReceiver&& f) noexcept : promise(Move(f.promise))
    {
    }
    FutureReceiver(FutureReceiver&) = delete;
    using receive_type = Tuple<Pure<Args>...>;

    Promise<receive_type> promise;

    void SetValue(receive_type&& value)
    {
        promise.SetValue(value);
    }
    void SetValue(receive_type& value)
    {
        promise.SetValue(value);
    }

    void SetError(ExceptionPtr ptr)
    {
        promise.SetException(ptr);
    }

    Future<receive_type> GetFuture()
    {
        return Future<receive_type>(promise.GetFuture());
    }
};

template <typename... Args>
struct FutureReceiver<Tuple<Args...>> : Receiver
{
    FutureReceiver() = default;
    FutureReceiver(FutureReceiver&& f) noexcept : promise(Move(f.promise))
    {
    }
    FutureReceiver(FutureReceiver& f) = delete;
    using receive_type = Tuple<Args...>;
    Promise<receive_type> promise;

    void SetValue(receive_type&& value)
    {
        promise.SetValue(value);
    }
    void SetValue(receive_type& value)
    {
        promise.SetValue(value);
    }

    void SetError(ExceptionPtr ptr)
    {
        promise.SetException(ptr);
    }

    Future<receive_type> GetFuture()
    {
        return Future<receive_type>(promise.GetFuture());
    }
};
} // namespace NExec
