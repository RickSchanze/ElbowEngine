//
// Created by Echo on 25-3-6.
//

#pragma once
#include "Common.h"
#include "Core/Async/Promise.h"
#include "Core/Base/TypeTraits.h"

namespace core::exec {

template <typename... Args> struct FutureReceiver : Receiver {
  FutureReceiver() = default;
  FutureReceiver(FutureReceiver &&f) : promise(Move(f.promise)) {}
  FutureReceiver(FutureReceiver &) = delete;
  using receive_type = std::tuple<Pure<Args>...>;

  Promise<receive_type> promise;

  void SetValue(receive_type &&value) { promise.SetValue(value); }
  void SetValue(receive_type &value) { promise.SetValue(value); }

  void SetError(std::exception_ptr ptr) { promise.SetException(ptr); }

  Future<receive_type> GetFuture() { return Future<receive_type>(promise.GetFuture()); }
};

template <typename... Args> struct FutureReceiver<std::tuple<Args...>> : Receiver {
  FutureReceiver() = default;
  FutureReceiver(FutureReceiver &&f) : promise(Move(f.promise)) {}
  FutureReceiver(FutureReceiver &f) = delete;
  using receive_type = std::tuple<Args...>;
  Promise<receive_type> promise;

  void SetValue(receive_type &&value) { promise.SetValue(value); }
  void SetValue(receive_type &value) { promise.SetValue(value); }

  void SetError(std::exception_ptr ptr) { promise.SetException(ptr); }

  Future<receive_type> GetFuture() { return Future<receive_type>(promise.GetFuture()); }
};

} // namespace core::exec