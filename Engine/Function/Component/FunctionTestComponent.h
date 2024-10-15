/**
 * @file FunctionTestComponent.h
 * @author Echo 
 * @Date 24-10-13
 * @brief 
 */

#pragma once
#include "CoreMacro.h"

#ifdef ENABLE_TEST

#include "Async/Coroutine/Task.h"
#include "Component.h"
#include "CoreDef.h"

namespace function::comp
{

/**
 * 用于各种功能测试的Component
 */
ECLASS()
class FunctionTestComponent : public TickableComponent
{
public:
    FunctionTestComponent();

    void BeginPlay() override;

    void Tick() override;

    async::coro::Task<void> TestWaitFormFrame();
    async::coro::Task<void> TestAwaitTask();

private:
    EPROPERTY()
    bool tested_ = false;

    std::deque<async::coro::Task<void>> tasks_;
};

}   // namespace function::comp

#endif
