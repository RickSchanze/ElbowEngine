//
// Created by Echo on 25-3-7.
//

#pragma once
#include "Common.h"
#include "Core/Async/IRunnable.h"
#include "Core/Base/TypeTraits.h"

#include <type_traits>

namespace core::exec {

template <typename OpType>
  requires std::is_base_of_v<Op, OpType>
struct ExecRunnable final : IRunnable {
  ExecRunnable(Pure<OpType>&) = delete;

  bool Run() override {
    if constexpr (std::same_as<decltype(std::declval<Pure<OpType>>().Start()), bool>) {
      return op_.Start();
    } else {
      op_.Start();
      return true;
    }
  }

  Pure<OpType> op_;
  ExecRunnable(Pure<OpType> &&op) : op_{Move(op)} {}
};

} // namespace core::exec
