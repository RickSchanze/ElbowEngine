//
// Created by Echo on 25-3-6.
//

#pragma once

#include "Core/Base/CoreTypeDef.h"
#include "concurrentqueue/blockingconcurrentqueue.h"

#if ELBOW_DEBUG
#include <queue>
#endif

#ifdef ELBOW_DEBUG
#define DEBUG_QUEUE 1
#else
#define DEBUG_QUEUE 0
#endif

namespace core {
template <typename T> class ConcurrentQueue {
public:
  void Enqueue(const T &t) {
#if DEBUG_QUEUE
    std::lock_guard lock(mutex_);
    queue_.push_back(t);
#else
    queue_.enqueue(t);
#endif
  }
  void Enqueue(T &&t) {
#if DEBUG_QUEUE
    std::lock_guard lock(mutex_);
    queue_.push_back(Move(t));
#else
    queue_.enqueue(std::move(t));
#endif
  }

  bool TryDequeue(T &t) { return queue_.try_dequeue(t); }
  void WaitDequeue(T &t) { queue_.wait_dequeue(t); }

  template <typename Rep, typename Period>
  void WaitDequeueTimed(T &t, const std::chrono::duration<Rep, Period> &timeout) {
    queue_.wait_dequeue_timed(t, timeout);
  }

  [[nodiscard]] UInt64 Size() const { return queue_.size_approx(); }

  [[nodiscard]] Bool IsEmpty() const { return Size() == 0; }

private:
  moodycamel::BlockingConcurrentQueue<T> queue_;
};
} // namespace core
