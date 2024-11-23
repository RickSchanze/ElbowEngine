/**
 * @file IRHIResource.h
 * @author Echo 
 * @Date 24-11-23
 * @brief 
 */

#pragma once

namespace platform::rhi
{
class IRHIResource {
public:
    virtual ~IRHIResource() = default;

    [[nodiscard]] virtual void* GetNativeHandle() const = 0;
};
}
