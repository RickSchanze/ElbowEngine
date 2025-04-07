//
// Created by Echo on 2025/3/22.
//
#pragma once
#include <thread>

#include "ConcurrentQueue.hpp"
#include "Core/Core.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "Core/String/String.hpp"


class IRunnable;
class Thread {
public:
    explicit Thread(bool is_game_thread = false) {
        if (!is_game_thread) {
            thread_ = std::thread([this] { Work(); });
        }
    }

    ~Thread() {
        Stop();
        Join();
    }

    ConcurrentQueue<SharedPtr<IRunnable>> &InternalGetTaskQueue() { return tasks_; }

    void SetName(StringView name);

    void AddRunnable(const SharedPtr<IRunnable> &runnable) { tasks_.Enqueue(runnable); }

    void Work(Int32 work_num = -1, bool persistent = true);
    void Stop();

    // 是否空闲, 检查任务列表里有没有任务
    bool Leisure() const { return !working_; }

    void Join();

private:
#ifdef ELBOW_DEBUG
    String name_;
#endif

    std::thread thread_;
    ConcurrentQueue<SharedPtr<IRunnable>> tasks_;
    std::atomic<bool> stopped_ = false;
    std::atomic<bool> working_ = false;
};

using ThreadId = std::thread::id;

inline ThreadId GetThisThreadId() { return std::this_thread::get_id(); }
