/**
 * @file IRHIResource.h
 * @author Echo 
 * @Date 24-4-19
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "RHI/Vulkan/VulkanCommon.h"

RHI_VULKAN_NAMESPACE_BEGIN

interface IRHIResource {
protected:
    struct ResourceProtected{};

public:
    virtual ~IRHIResource() = default;
    virtual void Destroy() = 0;
};

RHI_VULKAN_NAMESPACE_END
