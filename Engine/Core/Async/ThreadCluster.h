//
// Created by Echo on 24-12-12.
//

#pragma once
#include "Core/Base/EString.h"
#include "ITask.h"
#include "ThreadSafeQueue.h"

namespace core
{

enum class SlotType
{
    Game,
    Resource,
    Render,
    Audio,
    Other,
    Count,
};

class ThreadCluster
{
private:
    ThreadSafeQueue<SharedPtr<ITask>> task_queue_;
    Array<std::thread>      threads_;
    std::mutex              mutex_;
    bool                    stopping_ = false;

    void Work();

public:
    explicit ThreadCluster(size_t num_threads);

    void SetClusterName(core::StringView name);

    ThreadCluster(const ThreadCluster&)            = delete;
    ThreadCluster& operator=(const ThreadCluster&) = delete;

    ~ThreadCluster();

    void AddTask(SharedPtr<ITask> task);
};
}   // namespace core
