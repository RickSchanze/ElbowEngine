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
public:
    virtual ~IRHIResource() = default;
    virtual void Initialize() INTERFACE_METHOD;
    virtual void Finalize() INTERFACE_METHOD;
};

RHI_VULKAN_NAMESPACE_END