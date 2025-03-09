//
// Created by RickS on 24-12-8.
//
#pragma once
#include "Concept.h"
#include "Core/Base/CoreTypeDef.h"
#include "Core/Base/TypeTraits.h"

namespace core::exec {

template <typename... Args> struct JustSender : Sender {
  using value_type = std::tuple<Pure<Args>...>;
  std::tuple<Pure<Args>...> value;

  JustSender(Args &&...args) : value(std::make_tuple(Pure<Args>(Forward<Args>(args))...)) {}

  template <typename R> struct Operation : Op {
    using value_type = typename Pure<R>::receive_type;
    std::tuple<Pure<Args>...> v;
    Pure<R> r;

    void Start() noexcept {
      try {
        std::apply([&](auto &&...args) { r.SetValue(args...); }, value);
        r.SetValue(v);
      } catch (...) {
        r.SetError(std::current_exception());
      }
    }
  };

  template <typename R>
    requires std::same_as<value_type, typename R::receive_type>
  Operation<Pure<R>> Connect(R &&r) {
    return {value, Forward<R>(r)};
  }
};

struct VoidJustSender : Sender {
  using value_type = std::tuple<>;

  template <typename R> struct Operation : Op {
    Operation(R &&r) : r(Forward<R>(r)) {}

    using value_type = std::tuple<>;
    Pure<R> r;

    void Start() noexcept {
      try {
        r.SetValue(std::make_tuple());
      } catch (...) {
        r.SetError(std::current_exception());
      }
    }
  };

  template <typename R>
  Operation<Pure<R>> Connect(R &&r)
    requires std::same_as<std::tuple<>, typename Pure<R>::receive_type>
  {
    Operation<Pure<R>> rtn(Move(r));
    return rtn;
  }
};

template <typename... Args>
auto Just(Args... args)
  requires(sizeof...(Args) > 0)
{
  return JustSender<Pure<Args>...>(std::forward<Args>(args)...);
}

inline VoidJustSender Just() { return {}; }
} // namespace core::exec
