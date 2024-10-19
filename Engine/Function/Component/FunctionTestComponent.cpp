/**
 * @file FunctionTestComponent.cpp
 * @author Echo 
 * @Date 24-10-13
 * @brief 
 */

#include "FunctionTestComponent.h"

#ifdef ENABLE_TEST
#include "World/AsyncOperation/WaitForFrame.h"
#include "Async/Coroutine/Coroutine.h"

namespace function::comp
{
FunctionTestComponent::FunctionTestComponent()
{
    SetName(L"功能测试组件");
}

void FunctionTestComponent::BeginPlay()
{
    TickableComponent::BeginPlay();

}

void FunctionTestComponent::Tick()
{
    TickableComponent::Tick();
    // TestTask3().Forget();
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

async::coro::Task<int> FunctionTestComponent::TestReturnTask()
{
    LOG_INFO_ANSI_CATEGORY(Test.Coro, "Begin 测试带返回值的Task");
    co_await TestWaitFormFrame();
    LOG_INFO_ANSI_CATEGORY(Test.Coro, "End 测试带返回值的Task");
    co_return 12;
}

async::coro::Task<int> FunctionTestComponent::TestTask2()
{
    int a = co_await TestReturnTask();
    LOG_INFO_ANSI_CATEGORY(Test.Coro, "TestTask2: {}", a);
    co_return 15;
}

async::coro::Task<void> FunctionTestComponent::TestTask3()
{
    int a = co_await TestTask2();
    LOG_INFO_ANSI_CATEGORY(Test.Coro, "TestTask3: {}", a);
}

}   // namespace function::comp

#endif
