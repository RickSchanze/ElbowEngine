//
// Created by Echo on 25-3-8.
//

#pragma once
#include "Common.h"
#include <tuple>

namespace core::exec {

template <typename... ExecFutures> struct WhenAllExecFutureSender {
  using combined_type = std::tuple<typename ExecFutures::value_type...>;
  using value_type = typename Flatten<combined_type>::type;

  std::tuple<Pure<ExecFutures>...> futures;

  WhenAllExecFutureSender(ExecFutures &&...futures) : futures(std::make_tuple(Move(futures)...)) {}

  template <typename R> struct Operation : Op {
    std::tuple<Pure<ExecFutures>...> op_futures;
    Pure<R> next;

    Operation(Pure<R> &&r, std::tuple<Pure<ExecFutures>...> &&futures) : op_futures(Move(futures)), next(Move(r)) {}

    template <size_t I> bool Check() {
      if constexpr (I < sizeof...(ExecFutures)) {
        return std::get<I>(op_futures).Completed() && Check<I + 1>();
      } else {
        return true;
      }
    }

    template <size_t I, size_t Minus = 0> void CollectResultTuple(value_type &result) {
      if constexpr (I < sizeof...(ExecFutures)) {
        if constexpr (std::same_as<std::tuple_element_t<I, std::tuple<Pure<ExecFutures>...>>, std::tuple<>>) {
          CollectResultTuple<I + 1, Minus + 1>(result);
        } else {
          // TODO: 如果第i个是个多个参数的tuple, 又要将这个tuple展开遍历不能直接Get
          std::get<I - Minus>(result) = std::get<I>(op_futures).Get();
          CollectResultTuple<I + 1, Minus>(result);
        }
      }
    }

    bool Start() noexcept {
      if (Check<0>()) {
        value_type tuple;
        CollectResultTuple<0, 0>(tuple);
        next.SetValue(Move(tuple));
        return true;
      }
      return false;
    }
  };

  template <typename ReceiverType>
  auto Connect(ReceiverType &&r)
    requires std::same_as<value_type, typename Pure<ReceiverType>::receive_type>
  {
    return Operation<ReceiverType>(Move(r), Move(futures));
  }
};

template <typename... ExecFutures> auto WhenAllExecFuture(ExecFutures &&...futures) {
  return WhenAllExecFutureSender<Pure<ExecFutures>...>(Move(futures)...);
}

} // namespace core::exec
