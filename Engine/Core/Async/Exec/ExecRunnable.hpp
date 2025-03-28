//
// Created by Echo on 2025/3/22.
//
#pragma once
#include <type_traits>

#include "Common.hpp"
#include "Core/Async/IRunnable.hpp"
#include "Core/TypeAlias.hpp"
#include "Core/TypeTraits.hpp"


namespace exec {
template <typename OpType>
  requires IsBaseOf<Op, OpType>
struct ExecRunnable final : IRunnable {
  explicit ExecRunnable(Pure<OpType> &) = delete;

  bool Run() override {
    if constexpr (SameAs<decltype(std::declval<Pure<OpType>>().Start()), bool>) {
      return op_.Start();
    } else {
      op_.Start();
      return true;
    }
  }

  Pure<OpType> op_;
  explicit ExecRunnable(Pure<OpType> &&op) : op_{Move(op)} {}
};
} // namespace exec