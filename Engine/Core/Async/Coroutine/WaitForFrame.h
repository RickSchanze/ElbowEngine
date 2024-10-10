/**
 * @file WaitForFrame.h
 * @author Echo 
 * @Date 24-10-10
 * @brief 
 */

#pragma once
#include "Awaiter.h"

namespace async::coro
{

struct WaitForFrame : Awaiter<void>
{
    void AfterSuspend() override;
};

}
