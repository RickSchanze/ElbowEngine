//
// Created by Echo on 2025/3/22.
//

#pragma once
#include <condition_variable>
#include <queue>

template<typename T>
class ConcurrentQueue {
public:
    void Enqueue(const T &t) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(t);
        cond_.notify_one();
    }

    void Enqueue(T &&t) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(t));
        cond_.notify_one();
    }

    bool TryDequeue(T &t) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return false;
        }
        t = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    void WaitDequeue(T &t) {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this] { return !queue_.empty(); });
        t = std::move(queue_.front());
        queue_.pop();
    }

    template<typename Rep, typename Period>
    bool WaitDequeueTimed(T &t, const std::chrono::duration<Rep, Period> &timeout) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!cond_.wait_for(lock, timeout, [this] { return !queue_.empty(); })) {
            return false;
        }
        t = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    size_t Size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    bool IsEmpty() const { return Size() == 0; }

private:
    mutable std::mutex mutex_;
    std::condition_variable cond_;
    std::queue<T> queue_;
};
