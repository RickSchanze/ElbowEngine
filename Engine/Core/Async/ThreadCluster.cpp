//
// Created by Echo on 24-12-12.
//

#include "ThreadCluster.h"

#include "ThreadUtils.h"
void core::ThreadCluster::Work()
{
    std::unique_lock lock(mutex_);
    while (true)
    {
        ITask* task = nullptr;
        {
            if (stopping_ && task_queue_.Empty())
            {
                return;
            }
            task_queue_.TryPop(task);
        }
        if (task)
        {
            task->Execute();
            // TODO: 这里可能在主线程基于loop的协程时出现问题
            Delete(task);
        }
    }
}

core::ThreadCluster::ThreadCluster(size_t num_threads)
{
    for (size_t i = 0; i < num_threads; i++)
    {
        threads_.emplace_back(&ThreadCluster::Work, this);
    }
}

void core::ThreadCluster::SetClusterName(const StringView name)
{
    const String prefix = name;
    for (size_t i = 0; i < threads_.size(); i++)
    {
        String thread_name = prefix+ std::to_string(i);
        ThreadUtils::SetThreadName(threads_[i], thread_name);
    }
}

core::ThreadCluster::~ThreadCluster()
{
    {
        std::unique_lock lock(mutex_);
        stopping_ = true;
    }
    condition_.notify_all();
    for (auto& thread: threads_)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}

void core::ThreadCluster::AddTask(ITask* task)
{
    task_queue_.Push(task);
}