/**
 * @file IGraphicsPipeline.h
 * @author Echo 
 * @Date 24-7-11
 * @brief 
 */

#pragma once
#include "RHI/Vulkan/VulkanCommon.h"

RHI_VULKAN_NAMESPACE_BEGIN

class IGraphicsPipeline
{
public:
    virtual ~IGraphicsPipeline() = default;

    virtual vk::CommandBuffer GetCurrentCommandBuffer() const = 0;
};

class ImguiGraphicsPipeline : public IGraphicsPipeline
{
public:
    virtual void Draw() = 0;
};

RHI_VULKAN_NAMESPACE_END
