//
// Created by Echo on 25-3-8.
//

#pragma once
#include "Core/Async/Future.h"
#include <tuple>

namespace core::exec {

template <typename... Args> struct WhenAllFutureSender {
  using value_type = std::tuple<Pure<Args>...>;

  explicit WhenAllFutureSender(Future<Pure<Args>> &&...futures) : futures{Move(futures)...} {}

  std::tuple<Future<Pure<Args>>...> futures;

  template <typename R>
    requires(std::is_base_of_v<Receiver, Pure<R>> && std::same_as<value_type, typename Pure<R>::receive_type>)
  struct Op {
    std::tuple<Future<Pure<Args>>...> futures;
    Pure<R> r;

    template <size_t Index = 0> bool Check() {
      if constexpr (Index < sizeof(Args)) {
        if (std::get<Index>(futures).IsReady()) {
          return Check<Index + 1>();
        } else {
          return false;
        }
      }
      return true;
    }

    template <size_t Index = 0> void Fill(std::tuple<Pure<Args>...> &t) {
      if constexpr (Index < sizeof(Args)) {
        std::get<Index>(t) = std::get<Index>(futures).Get();
        Fill<Index + 1>(t);
      }
    }

    std::tuple<Pure<Args>...> Get() {
      std::tuple<Pure<Args>...> t;
      Fill(t);
      return t;
    }

    bool Start() {
      if (Check()) {
        r.SetValue(Get());
        return true;
      }
      return false;
    }
  };

  template <typename R>
    requires std::is_base_of_v<Receiver, Pure<R>> && std::same_as<value_type, typename Pure<R>::receive_type>
  Op<Pure<R>> Connect(R &&r) {
    Op<Pure<R>> op;
    op.futures = Move(futures);
    op.r = r;
    return op;
  }
};

template <typename... Args> WhenAllFutureSender<Pure<Args>...> WhenAllFuture(Args &&...futures) {
  return WhenAllFutureSender<Pure<Args>...>(Move(futures)...);
}

} // namespace core::exec
