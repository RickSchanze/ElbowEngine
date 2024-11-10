/**
 * @file IRHIResource.h
 * @author Echo 
 * @Date 24-11-9
 * @brief 
 */

#pragma once

namespace platform::rhi
{
class IRHIResource
{
public:
    virtual ~IRHIResource() = default;

    [[nodiscard]] virtual void* GetNativePtr() const = 0;
    [[nodiscard]] virtual bool  IsValid() const      = 0;
};
}   // namespace platform::rhi
