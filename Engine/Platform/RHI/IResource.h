/**
 * @file IRHIResource.h
 * @author Echo 
 * @Date 24-11-23
 * @brief 
 */

#pragma once

namespace platform::rhi
{
class IResource {
public:
    virtual ~IResource() = default;

    [[nodiscard]] virtual void* GetNativeHandle() const = 0;
};
}
