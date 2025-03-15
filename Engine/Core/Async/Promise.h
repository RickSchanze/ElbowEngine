//
// Created by Echo on 25-3-6.
//

#pragma once

#include "Core/Log/Logger.h"
#include "Future.h"

namespace core {
template <typename T> struct Promise {
public:
  Promise(Promise &) = delete;
  Promise(Promise &&p) noexcept : promise_(Move(p.promise_)) {}
  Promise() = default;
  explicit Promise(std::promise<T> &) = delete;
  Promise(std::promise<T> &&p) : promise_(Move(p)) {}

  Future<T> GetFuture() { return Future<T>(promise_.get_future()); }

  void SetValue(T &result) {
    promise_.set_value(result);
  }
  void SetValue(T &&result) {
    promise_.set_value(Move(result));
  }
  void SetException(std::exception_ptr ptr) {
    try {
      std::rethrow_exception(ptr);
    } catch (std::exception &ex) {
      LOGGER.Critical("Async", "Exception: {}", ex.what());
    }
  }

private:
  std::promise<T> promise_;
};

template <> struct Promise<void> {
  Promise(Promise &) = delete;
  Promise(Promise &&p) noexcept : promise_(Move(p.promise_)) {}
  Promise() = default;
  explicit Promise(std::promise<void> &) = delete;
  Promise(std::promise<void> &&p) : promise_(Move(p)) {}

  void SetValue() { promise_.set_value(); }
  void SetException(const std::exception_ptr &ptr) { promise_.set_exception(ptr); }

  Future<void> GetFuture() { return Future<void>(promise_.get_future()); }

private:
  std::promise<void> promise_;
};
} // namespace core
