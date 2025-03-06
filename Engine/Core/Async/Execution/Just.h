//
// Created by RickS on 24-12-8.
//
#pragma once
#include "Concept.h"
#include "Core/Base/CoreTypeDef.h"

namespace core::exec {

template <typename T> struct JustSender : Sender {
  using value_type = Pure<T>;
  Pure<T> value;

  template <typename R> struct Operation {
    Pure<T> v;
    R r;

    void Start() noexcept {
      try {
        r.SetValue(v);
      } catch (...) {
        r.SetError(std::current_exception());
      }
    }
  };

  template <typename R> Operation<R> Connect(R &&r) { return {value, Forward<R>(r)}; }
};

struct VoidJustSender : Sender {
  using value_type = void;

  template <typename R> struct Operation {
    R r;

    void Start() noexcept {
      try {
        r.SetValue();
      } catch (...) {
        r.SetError(std::current_exception());
      }
    }
  };

  template <typename R> Operation<R> Connect(R &&r) { return {Forward<R>(r)}; }
};

template <typename T> JustSender<T> Just(T &&value) { return {Forward<T>(value)}; }

inline VoidJustSender Just() { return {}; }
} // namespace core::exec
