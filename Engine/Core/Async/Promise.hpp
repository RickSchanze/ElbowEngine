//
// Created by Echo on 2025/3/22.
//


#pragma once
#include <future>

#include "Core/Logger/Logger.hpp"
#include "Core/Misc/Other.hpp"
#include "Future.hpp"

template <typename T> struct Promise {

  Promise(Promise &) = delete;
  Promise(Promise &&p) noexcept : promise_(Move(p.promise_)) {}
  Promise() = default;
  explicit Promise(std::promise<T> &) = delete;
  Promise(std::promise<T> &&p) : promise_(Move(p)) {}

  Future<T> GetFuture() { return Future<T>(promise_.get_future()); }

  void SetValue(T &result) { promise_.set_value(result); }
  void SetValue(T &&result) { promise_.set_value(Move(result)); }
  void SetException(const std::exception_ptr &ptr) {
    try {
      std::rethrow_exception(ptr);
    } catch (std::exception &ex) {
      Log(Fatal) << "Async Execption"_es + ex.what();
      Exit(1);
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

  Future<void> GetFuture() { return promise_.get_future(); }

private:
  std::promise<void> promise_;
};
