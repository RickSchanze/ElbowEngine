/**
 * @file IGraphicsPipeline.h
 * @author Echo 
 * @Date 24-7-11
 * @brief 
 */

#pragma once
#include "RHI/Vulkan/VulkanCommon.h"

namespace rhi::vulkan
{
class IGraphicsPipeline
{
public:
    virtual ~IGraphicsPipeline() = default;
};

class ImguiGraphicsPipeline : public IGraphicsPipeline
{
public:
    virtual void Draw(vk::CommandBuffer cb) = 0;
    virtual void Rebuild(int w, int h) = 0;
};
}
