/**
 * @file CoroutineCommon.h
 * @author Echo 
 * @Date 24-10-9
 * @brief 
 */

#pragma once
#include "IExecutor.h"
#include <coroutine>

namespace async::coro
{
template<typename T, EExecutorType = EExecutorType::MainThread>
struct Task;

template<typename T, EExecutorType = EExecutorType::MainThread>
struct Promise;

template<typename T>
struct Awaiter;

template <typename T>
struct Result;

template <typename T>
struct TaskAwaiter;
}
