/**
 * @file MainThreadExecutor.cpp
 * @author Echo 
 * @Date 24-10-9
 * @brief 
 */

#include "MainThreadExecutor.h"

async::coro::MainThreadExecutor::MainThreadExecutor()
{
    updaters_.reserve(16);
}

void async::coro::MainThreadExecutor::Install(const AwaiterUpdater& updater)
{

}
