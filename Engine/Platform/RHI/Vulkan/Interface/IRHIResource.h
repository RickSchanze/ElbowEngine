/**
 * @file IRHIResource.h
 * @author Echo 
 * @Date 24-4-19
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "RHI/Vulkan/VulkanCommon.h"

namespace rhi::vulkan
{
class IRHIResource
{
protected:
    struct ResourceProtected
    {
    };

public:
    virtual ~IRHIResource() = default;
    virtual void Destroy()  = 0;
};
}   // namespace rhi::vulkan
