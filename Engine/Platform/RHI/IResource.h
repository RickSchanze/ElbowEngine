/**
 * @file IResource.h
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

    [[nodiscard]] virtual bool IsValid() const;
};
}
