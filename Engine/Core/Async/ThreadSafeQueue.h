//
// Created by Echo on 24-12-12.
//

#pragma once
#include "Core/Base/CoreTypeDef.h"
#include "Core/Profiler/ProfileMacro.h"
#include <mutex>
#include <queue>

using namespace std::chrono_literals;

namespace core {
/**
 * 线程安全队列
 * @tparam LockFree 是否无锁, 目前还没有无锁实现
 */
template <typename T, bool LockFree = false> class ThreadSafeQueue;

template <typename T> class ThreadSafeQueue<T, true> {};

template <typename T> class ThreadSafeQueue<T, false> {
private:
  std::queue<T> queue_;
  mutable DECLARE_TRACEABLE_MUTEX(std::mutex, mutex_, "队列锁");
  std::condition_variable not_empty_;

public:
  ThreadSafeQueue() = default;

  ThreadSafeQueue(const ThreadSafeQueue &) = delete;
  ThreadSafeQueue &operator=(const ThreadSafeQueue &) = delete;

  void Push(const T &element) {
    PROFILE_SCOPE_AUTO;
    std::lock_guard lock(mutex_);
    queue_.push(element);
    not_empty_.notify_one(); // 通知等待的线程
  }

  T Pop() {
    PROFILE_SCOPE_AUTO;
    std::unique_lock lock(mutex_);
    not_empty_.wait(lock, [this] { return !queue_.empty(); });
    T element = std::move(queue_.front());
    queue_.pop();
    return element;
  }

  /**
   * 从队列出取元素
   * @param element
   * @param timeout 超时时间 0ms表示永不超时
   * @return
   */
  bool TryPop(T &element, std::chrono::milliseconds timeout = 0ms) {
    PROFILE_SCOPE_AUTO;
    std::unique_lock lock(mutex_);
    if (timeout == 0ms) {
      not_empty_.wait(lock, [this] { return !queue_.empty(); });
    } else if (!not_empty_.wait_for(lock, timeout, [this] { return !queue_.empty(); })) {
      return false;
    }
    element = std::move(queue_.front());
    queue_.pop();
    return true;
  }

  bool Empty() const {
    PROFILE_SCOPE_AUTO;
    std::unique_lock lock(mutex_);
    return queue_.empty();
  }

  UInt64 Count() const {
    PROFILE_SCOPE_AUTO;
    std::unique_lock lock(mutex_);
    return queue_.size();
  }
};
}   // namespace core
