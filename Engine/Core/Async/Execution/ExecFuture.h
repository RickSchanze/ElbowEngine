//
// Created by Echo on 25-3-9.
//

#pragma once
#include "Core/Async/Future.h"

namespace core::exec {

template <typename... Args> struct ExecFuture {
  using value_type = std::tuple<Args...>;
  Future<std::tuple<Args...>> future;

  ExecFuture(Future<std::tuple<Args...>> &&f) : future(Move(f)) {}

  [[nodiscard]] bool Completed() const { return future.Completed(); }

  template <size_t Index = 0> auto Get() {
    if constexpr (sizeof...(Args) == 0) {
      Wait();
    } else {
      return std::get<Index>(future.Get());
    }
  }

  void Wait() const { future.Wait(); }

  template <typename Rep, typename Period> bool WaitFor(const std::chrono::duration<Rep, Period> &timeout) {
    return future.WaitFor(timeout);
  }
};

template <typename... Args> auto MakeExecFuture(Args &&...args) {
  return MakeReadyFuture(std::make_tuple(Forward<Args>(args)...));
}

} // namespace core::exec
