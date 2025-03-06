//
// Created by Echo on 25-3-6.
//

#pragma once

#include "Core/Base/TypeTraits.h"
#include "Func/Render/ElbowRenderPipeline.h"

#include <exception>

namespace core::exec {
template <typename Receiver, typename Func> struct ThenReceiver {
  Receiver next;
  Func f;

  template <typename V> void SetValue(V &&v) noexcept {
    try {
      next.SetValue(std::invoke(f, Forward<V>(v)));
    } catch (...) {
      next.SetError(std::current_exception());
    }
  }

  void SetError(std::exception_ptr ptr) noexcept { next.SetError(ptr); }
};

template <typename SenderT, typename F> struct ThenSender : Sender {
  using value_type = FunctionReturnType<F>;
  SenderT sender;
  F f;

  template <typename Receiver> auto Connect(Receiver &&r) {
    return sender.Connect(ThenReceiver<Pure<Receiver>, F>{Forward<Pure<Receiver>>(r), f});
  }
};

struct Then1Type {
  template <typename F> auto operator()(F &&f) const noexcept;

  template <typename SenderT, typename F> auto operator()(SenderT &&s, F &&f) {
    return ThenSender{Forward<SenderT>(s), Forward<F>(f)};
  }
};

template <typename F> struct Then1Closure {
  F f_;
  template <typename SenderT>
  friend auto operator|(SenderT &&s, Then1Closure &&t) -> ThenSender<SenderT, F>
      // 下面一大坨是约束, 即要求s的输出参数value_type和F的输入参数一致
    requires(std::same_as<std::conditional_t<std::is_same_v<typename SenderT::value_type, void>, std::tuple<>,
                                             std::tuple<typename SenderT::value_type>>,
                          FunctionArgsAsTuple<F>>)
  {
    ThenSender<SenderT, F> rtn{};
    rtn.f = Forward<F>(t.f_);
    rtn.sender = Forward<SenderT>(s);
    return rtn;
  }
};
template <typename F> auto Then1Type::operator()(F &&f) const noexcept { return Then1Closure{Forward<F>(f)}; }

inline Then1Type Then1;
} // namespace core::exec
