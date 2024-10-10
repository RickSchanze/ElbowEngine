/**
 * @file Task.cpp
 * @author Echo 
 * @Date 24-10-7
 * @brief 
 */

#include "Task.h"

namespace async::coro
{

Task<void>::~Task()
{
    if (handle_)
    {
        handle_.destroy();
    }
}

bool Task<void>::IsCompleted() const
{
    return handle_.promise().IsCompleted();
}

}   // namespace async::coro
