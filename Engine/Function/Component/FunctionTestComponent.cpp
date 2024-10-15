/**
 * @file FunctionTestComponent.cpp
 * @author Echo 
 * @Date 24-10-13
 * @brief 
 */

#include "FunctionTestComponent.h"

#ifdef ENABLE_TEST
#include "World/AsyncOperation/WaitForFrame.h"
#include "Async/Coroutine/Task.h"
#include "Async/Coroutine/TaskAwaiter.h"

namespace function::comp
{
FunctionTestComponent::FunctionTestComponent()
{
    SetName(L"功能测试组件");
}

void FunctionTestComponent::BeginPlay()
{
    TickableComponent::BeginPlay();
    TestAwaitTask().Forget();
}

void FunctionTestComponent::Tick()
{
    TickableComponent::Tick();
}

async::coro::Task<void> FunctionTestComponent::TestWaitFormFrame()
{
    using namespace async::coro;
    LOG_INFO_ANSI_CATEGORY(Test.Coro, "测试等待几帧的协程");
    LOG_INFO_ANSI_CATEGORY(Test.Coro, "当前帧: {}", g_engine_statistics.frame_count);
    co_await WaitForFrame(3);
    LOG_INFO_ANSI_CATEGORY(Test.Coro, "等待3帧后的帧数: {}", g_engine_statistics.frame_count);
}

async::coro::Task<void> FunctionTestComponent::TestAwaitTask()
{
    LOG_INFO_ANSI_CATEGORY(Test.Coro, "测试AwaitTaskBegin");
    co_await TestWaitFormFrame();
    LOG_INFO_ANSI_CATEGORY(Test.Coro, "测试AwaitTaskEnd");
}

}   // namespace function::comp

#endif
