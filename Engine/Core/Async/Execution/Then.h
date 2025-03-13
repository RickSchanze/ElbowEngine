//
// Created by Echo on 25-3-6.
//

#pragma once

#include "Common.h"
#include "Core/Base/TypeTraits.h"
#include <exception>

namespace core::exec {
template <typename ReceiverType, typename F> struct ThenReceiver : exec::Receiver {
  using receive_type = FunctionArgsAsTuple<Pure<F>>;
  Pure<ReceiverType> next;
  F f;

  ThenReceiver(ReceiverType &&r, F &&func) : next(Move(r)), f(Forward<F>(func)) {}

  void SetValue(receive_type &&v) noexcept {
    try {
      if constexpr (std::same_as<FunctionReturnType<Pure<Pure<F>>>, void>) {
        std::apply(
            [this](auto &&...args) {
              f(args...);
              next.SetValue(std::make_tuple());
            },
            v);
      } else {
        std::apply([this](auto &&...args) { next.SetValue(std::make_tuple(f(args...))); }, v);
      }
    } catch (...) {
      next.SetError(std::current_exception());
    }
  }

  void SetError(std::exception_ptr ptr) noexcept { next.SetError(ptr); }
};

template <typename ReceiverType, typename F> struct VoidThenReceiver {
  using receive_type = std::tuple<>;
  static_assert(std::same_as<FunctionArgsAsTuple<Pure<F>>, receive_type>, "void receiver type not match");

  Pure<ReceiverType> next;
  F f;

  VoidThenReceiver(ReceiverType &&r, F &&func) : next(Move(r)), f(Forward<F>(func)) {}
  VoidThenReceiver(VoidThenReceiver &&r) : next(Move(r.next)), f(Move(r.f)) {}
  VoidThenReceiver(const VoidThenReceiver &r) = delete;

  void SetValue(const std::tuple<> &) noexcept {
    try {
      if constexpr (std::same_as<FunctionReturnType<Pure<F>>, void>) {
        next.SetValue(std::make_tuple());
      } else {
        next.SetValue(std::make_tuple(f()));
      }
    } catch (...) {
      next.SetError(std::current_exception());
    }
  }

  void SetError(std::exception_ptr ptr) noexcept { next.SetError(ptr); }
};

template <typename SenderT, typename F> struct ThenSender : exec::Sender {
  using wrapped_type_ = typename WrapTuple<FunctionReturnType<Pure<F>>>::type;
  using value_type = std::conditional_t<std::same_as<wrapped_type_, std::tuple<void>>, std::tuple<>, wrapped_type_>;
  using input_type = FunctionArgsAsTuple<Pure<F>>;

  ThenSender(SenderT &&s, F &&f) : sender(Forward<SenderT>(s)), f(Forward<F>(f)) {}

  Pure<SenderT> sender;
  F f;

  template <typename ReceiverType>
  auto Connect(ReceiverType &&r)
    requires std::same_as<value_type, typename Pure<ReceiverType>::receive_type>
  {
    if constexpr (std::same_as<input_type, std::tuple<>>) {
      return Move(sender.Connect(VoidThenReceiver<Pure<ReceiverType>, F>(Move(r), Forward<F>(f))));
    } else {
      ThenReceiver<Pure<ReceiverType>, F> receiver(Move(r), Forward<F>(f));
      return Move(sender.Connect(receiver));
    }
  }
};

struct ThenType {
  template <typename F> auto operator()(F &&f) const noexcept;

  template <typename SenderT, typename F> auto operator()(SenderT &&s, F &&f) {
    return ThenSender{Forward<SenderT>(s), Forward<F>(f)};
  }
};

template <typename F> struct ThenClosure {
  F f_;
  template <typename SenderT>
  friend auto operator|(SenderT &&s, ThenClosure &&t) -> ThenSender<Pure<SenderT>, F>
      // 下面一大坨是约束, 即要求s的输出参数value_type和F的输入参数一致
    requires(std::same_as<typename Pure<SenderT>::value_type, FunctionArgsAsTuple<F>>)
  {
    ThenSender<SenderT, F> rtn(Forward<SenderT>(s), Forward<F>(t.f_));
    return rtn;
  }
};
template <typename F> auto ThenType::operator()(F &&f) const noexcept { return ThenClosure{Forward<F>(f)}; }

inline ThenType Then;
} // namespace core::exec
