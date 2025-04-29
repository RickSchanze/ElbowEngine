//
// Created by Echo on 2025/3/22.
//
#pragma once
#include <future>

#include "Core/TypeAlias.hpp"

template <typename T> struct Future {
  Future(Future &f) = delete;
  Future(Future &&f) noexcept = default;
  explicit Future(std::future<T> &) = delete;
  Future(std::future<T> &&f) : future_(Move(f)) {}
  Future() = default;

  T Get() { return future_.get(); }
  T operator*() { return future_.get(); }
  void Wait() const { future_.wait(); }

  template <typename Rep, typename Period> bool WaitFor(const std::chrono::duration<Rep, Period> &timeout) {
    auto state = future_.wait_for(timeout);
    return state == std::future_status::ready;
  }

  bool IsCompleted() const { return future_.wait_for(std::chrono::seconds(0)) == std::future_status::ready; }

  Future &operator=(Future &&) = default;

private:
  std::future<T> future_;
};

template <> struct Future<void> {
  Future() = default;
  Future(Future &f) = delete;
  Future(Future &&f) noexcept : future_(Move(f.future_)) {}
  explicit Future(std::future<void> &) = delete;
  Future(std::future<void> &&f) noexcept : future_(Move(f)) {}
  void Get() { future_.get(); }
  void Wait() const { future_.wait(); }

  template <typename Rep, typename Period> bool WaitFor(const Duration<Rep, Period> &timeout) {
    auto state = future_.wait_for(timeout);
    return state == std::future_status::ready;
  }

  bool IsCompleted() const { return future_.wait_for(std::chrono::milliseconds(1)) == std::future_status::ready; }

private:
  std::future<void> future_;
};

template <typename T> Future<Pure<T>> MakeReadyFuture(T &&t) {
  std::promise<Pure<T>> promise;
  promise.set_value(t);
  return Future<Pure<T>>(promise.get_future());
}

inline Future<void> MakeReadyFuture() {
  std::promise<void> promise;
  promise.set_value();
  return {promise.get_future()};
}