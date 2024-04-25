/**
 * @file IRenderPassProducer.h
 * @author Echo 
 * @Date 24-4-25
 * @brief 
 */

#pragma once
#include "RHI/Vulkan/VulkanCommon.h"

RHI_VULKAN_NAMESPACE_BEGIN

interface IRenderPassProducer{
public:
    virtual ~IRenderPassProducer() = default;

    virtual vk::RenderPassCreateInfo GetRenderPassCreateInfo() INTERFACE_METHOD;
};

RHI_VULKAN_NAMESPACE_END
