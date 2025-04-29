//
// Created by Echo on 2025/3/22.
//

#pragma once
#include "Common.hpp"
#include "Core/Misc/Other.hpp"

namespace exec {
template <typename ReceiverType, typename F> struct ThenReceiver : exec::Receiver {
  using receive_type = Traits::FunctionArgsAsTuple<Pure<F>>;
  Pure<ReceiverType> next;
  F f;

  ThenReceiver(ReceiverType &&r, F &&func) : next(Move(r)), f(Forward<F>(func)) {}

  void SetValue(receive_type &&v) noexcept {
    try {
      if constexpr (Traits::SameAs<Traits::FunctionReturnType<Pure<Pure<F>>>, void>) {
        Apply(
            [this](auto &&...args) {
              f(args...);
              next.SetValue(MakeTuple());
            },
            v);
      } else {
        Apply([this](auto &&...args) { next.SetValue(MakeTuple(f(args...))); }, v);
      }
    } catch (...) {
      next.SetError(CurrentException());
    }
  }

  void SetError(ExceptionPtr ptr) noexcept { next.SetError(ptr); }
};

template <typename ReceiverType, typename F> struct VoidThenReceiver {
  using receive_type = Tuple<>;
  static_assert(Traits::SameAs<Traits::FunctionArgsAsTuple<Pure<F>>, receive_type>, "void receiver type not match");

  Pure<ReceiverType> next;
  F f;

  VoidThenReceiver(ReceiverType &&r, F &&func) : next(Move(r)), f(Forward<F>(func)) {}
  VoidThenReceiver(VoidThenReceiver &&r) noexcept : next(Move(r.next)), f(Move(r.f)) {}
  VoidThenReceiver(const VoidThenReceiver &r) = delete;

  void SetValue(const Tuple<> &) noexcept {
    try {
      if constexpr (Traits::SameAs<Traits::FunctionReturnType<Pure<F>>, void>) {
        f();
        next.SetValue(MakeTuple());
      } else {
        next.SetValue(MakeTuple(f()));
      }
    } catch (...) {
      next.SetError(CurrentException());
    }
  }

  void SetError(ExceptionPtr ptr) noexcept { next.SetError(ptr); }
};

template <typename SenderT, typename F> struct ThenSender : exec::Sender {
  using wrapped_type_ = typename Traits::WrapTuple<Traits::FunctionReturnType<Pure<F>>>::type;
  using value_type = Traits::Conditional<Traits::SameAs<wrapped_type_, Tuple<void>>, Tuple<>, wrapped_type_>;
  using input_type = Traits::FunctionArgsAsTuple<Pure<F>>;

  ThenSender(SenderT &&s, F &&f) : sender(Forward<SenderT>(s)), f(Forward<F>(f)) {}

  Pure<SenderT> sender;
  F f;

  template <typename ReceiverType>
  auto Connect(ReceiverType &&r)
    requires Traits::SameAs<value_type, typename Pure<ReceiverType>::receive_type>
  {
    if constexpr (Traits::SameAs<input_type, Tuple<>>) {
      return Move(sender.Connect(VoidThenReceiver<Pure<ReceiverType>, F>(Move(r), Forward<F>(f))));
    } else {
      ThenReceiver<Pure<ReceiverType>, F> receiver(Move(r), Forward<F>(f));
      return Move(sender.Connect(receiver));
    }
  }
};

struct ThenType {
  template <typename F> auto operator()(F &&f) const noexcept;

  template <typename SenderT, typename F> auto operator()(SenderT &&s, F &&f) { return ThenSender{Forward<SenderT>(s), Forward<F>(f)}; }
};

template <typename F> struct ThenClosure {
  F f_;
  template <typename SenderT>
  friend auto operator|(SenderT &&s, ThenClosure &&t) -> ThenSender<Pure<SenderT>, F>
      // 下面一大坨是约束, 即要求s的输出参数value_type和F的输入参数一致
    requires(Traits::SameAs<typename Pure<SenderT>::value_type, Traits::FunctionArgsAsTuple<F>>)
  {
    ThenSender<SenderT, F> rtn(Forward<SenderT>(s), Forward<F>(t.f_));
    return rtn;
  }
};
template <typename F> auto ThenType::operator()(F &&f) const noexcept { return ThenClosure{Forward<F>(f)}; }

inline ThenType Then;
} // namespace exec
