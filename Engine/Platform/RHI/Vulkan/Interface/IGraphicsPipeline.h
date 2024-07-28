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
};

class ImguiGraphicsPipeline : public IGraphicsPipeline
{
public:
    virtual void Draw(vk::CommandBuffer cb) = 0;
};

RHI_VULKAN_NAMESPACE_END
