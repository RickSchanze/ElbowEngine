//
// Created by Echo on 2025/3/22.
//

#pragma once
#include "Common.hpp"
#include "Core/Misc/Other.hpp"

namespace exec {
template <typename... Args> struct JustSender : Sender {
  using value_type = Tuple<Pure<Args>...>;
  Tuple<Pure<Args>...> value;

  JustSender(Args &&...args) : value(MakeTuple(Pure<Args>(Forward<Args>(args))...)) {}

  template <typename R> struct Operation : Op {
    using value_type = typename Pure<R>::receive_type;
    Tuple<Pure<Args>...> v;
    Pure<R> r;

    void Start() noexcept {
      try {
        Apply([&](auto &&...args) { r.SetValue(args...); }, v);
      } catch (...) {
        r.SetError(CurrentException());
      }
    }
  };

  template <typename R>
    requires SameAs<value_type, typename R::receive_type>
  Operation<Pure<R>> Connect(R &&r) {
    return {value, Forward<R>(r)};
  }
};

struct VoidJustSender : Sender {
  using value_type = Tuple<>;

  template <typename R> struct Operation : Op {
    Operation(R &&r) : r(Move(r)) {}
    Operation(R &) = delete;

    using value_type = Tuple<>;
    Pure<R> r;

    void Start() noexcept {
      try {
        r.SetValue(MakeTuple());
      } catch (...) {
        r.SetError(CurrentException());
      }
    }
  };

  template <typename R>
  Operation<Pure<R>> Connect(R &&r)
    requires SameAs<Tuple<>, typename Pure<R>::receive_type>
  {
    Operation<Pure<R>> rtn(Move(r));
    return Move(rtn);
  }
};

template <typename... Args>
auto Just(Args... args)
  requires(sizeof...(Args) > 0)
{
  return JustSender<Pure<Args>...>(Forward<Args>(args)...);
}

inline VoidJustSender Just() { return {}; }
} // namespace exec
