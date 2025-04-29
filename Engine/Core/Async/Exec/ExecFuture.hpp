//
// Created by Echo on 2025/3/22.
//

#pragma once
#include "Core/Async/Future.hpp"
#include "Core/TypeAlias.hpp"

namespace exec {
    template<typename... Args>
    struct ExecFuture {
        using value_type = Tuple<Args...>;
        Future<Tuple<Args...>> future;

        ExecFuture(Future<Tuple<Args...>> &&f) { future = Move(f); }

        bool IsCompleted() const { return future.IsCompleted(); }

        template<size_t Index = 0>
        auto Get() {
            if constexpr (sizeof...(Args) == 0) {
                future.Get();
            } else {
                return ::Get<Index>(future.Get());
            }
        }

        void Wait() const { future.Wait(); }

        template<typename Rep, typename Period>
        bool WaitFor(const Duration<Rep, Period> &timeout) {
            return future.WaitFor(timeout);
        }
    };

    template<typename... Args>
    auto MakeExecFuture(Args &&...args) {
        return MakeReadyFuture(MakeTuple(Forward<Args>(args)...));
    }
} // namespace exec
