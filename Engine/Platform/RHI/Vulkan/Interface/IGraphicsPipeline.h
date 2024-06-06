/**
 * @file IGraphicsPipeline.h
 * @author Echo 
 * @Date 24-5-10
 * @brief 
 */

#pragma once
#include "CoreGlobal.h"
#include "RHI/Vulkan/Render/CommandPool.h"

interface IGraphicsPipeline {
public:
    virtual ~IGraphicsPipeline()                                                        = default;
    // clang-format off
    virtual void SubmitGraphicsQueue(
        int       CurrentImageIndex,
        vk::Queue InGraphicsQueue,
        TArray<vk::Semaphore> InWaitSemaphores,
        TArray<vk::Semaphore> InSingalSemaphores,
        vk::Fence InFrameFence
    ) = 0;
    // clang-format on

    // 窗口大小改变时需要调用此函数
    virtual void Rebuild() = 0;
};
