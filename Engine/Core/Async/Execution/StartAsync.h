//
// Created by Echo on 24-12-15.
// TODO: shared_ptr的自定义内存分配
//

#pragma once
#include "Common.h"
#include "Core/Async/ThreadUtils.h"
#include "Core/Base/CoreTypeDef.h"
#include "Core/Base/Optional.h"
#include "Core/Base/TypeTraits.h"

namespace core::exec {

template <typename T> struct AsyncResult {
  std::atomic<bool> ready = false;
  std::exception_ptr exception{};
  Optional<T> data{};

  void SetException(const std::exception_ptr &_exception) { exception = _exception; }

  std::exception_ptr GetException() { return exception; }

  void SetDone() { ready.store(true); }

  bool IsDone() const { return ready.load(); }

  bool IsCanceled() const { return IsDone() && !data; }

  void SetData(const T &_data) {
    data = _data;
    ready.store(true);
  }

  Optional<T> GetValue() {
    Wait();
    if (exception) {
      std::rethrow_exception(exception);
    }
    return data;
  }

  AsyncResult &Wait() {
    while (!IsDone()) {
      ThreadUtils::YieldThread();
    }
    return *this;
  }
};

template <typename... Args> using AsyncResultHandle = SharedPtr<AsyncResult<std::tuple<Pure<Args>...>>>;

template <typename... Args> struct AsyncReceiver {
  using ReceiveTypes = std::tuple<Args...>;

  std::shared_ptr<AsyncResult<ReceiveTypes>> data;

  template <typename Receiver>
    requires std::is_same_v<Pure<Receiver>, AsyncReceiver>
  friend auto TagInvoke(SetValueType, Receiver &&r, Args &&...vs) noexcept {
    r.data->SetData(std::make_tuple(std::forward<Args>(vs)...));
  }

  template <typename Receiver, typename E>
    requires std::is_same_v<std::remove_cvref_t<Receiver>, AsyncReceiver>
  friend auto TagInvoke(SetErrorType, Receiver &&r, E &&e) noexcept {
    r.data->SetException(e);
  }

  template <typename Receiver>
    requires std::is_same_v<std::remove_cvref_t<Receiver>, AsyncReceiver>
  friend auto TagInvoke(SetDoneType, Receiver &&r) noexcept {
    r.data->SetDone();
  }
};

template <typename... Args> struct AsyncWrapper;

template <typename... Args> struct AsyncWrapper<std::tuple<Args...>> {
  using ReceiverType = AsyncReceiver<Args...>;
};

template <> struct AsyncWrapper<void> {
  using ReceiverType = AsyncReceiver<>;
};

#define NULL_ASYNC_RESULT_HANDLE nullptr

template <typename... Args> auto MakeAsyncResult(Args &&...args) {
  auto holder = MakeShared<AsyncResult<std::tuple<Pure<Args>...>>>();
  holder->SetData(std::make_tuple(Forward<Args>(args)...));
  return holder;
}

struct StartAsyncType {
  template <typename Sender> auto operator()(Sender sender) const {
    using ValueTypes = typename SenderTraits<Sender>::ValueTypes;
    typename AsyncWrapper<ValueTypes>::ReceiverType receiver;

    if constexpr (std::is_same_v<ValueTypes, void>) {
      auto result_handle = MakeShared<AsyncResult<std::tuple<>>>();
      receiver.data = result_handle;
      auto op = Connect(std::move(sender), receiver);
      Start(op);
      return result_handle;
    } else {
      auto result_handle = MakeShared<AsyncResult<ValueTypes>>();
      receiver.data = result_handle;
      auto op = Connect(std::move(sender), receiver);
      Start(op);
      return result_handle;
    }
  }
};

inline constexpr StartAsyncType StartAsync{};
} // namespace core::exec
