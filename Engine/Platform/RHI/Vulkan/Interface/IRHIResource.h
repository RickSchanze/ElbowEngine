/**
 * @file IRHIResource.h
 * @author Echo 
 * @Date 24-4-19
 * @brief 
 */

#pragma once

namespace rhi::vulkan
{
class IRHIResource
{
public:
    virtual ~IRHIResource() = default;
    virtual void Destroy()  = 0;
};
}   // namespace rhi::vulkan
