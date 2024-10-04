/**
 * @file CommandBuffer.h
 * @author Echo 
 * @Date 24-9-21
 * @brief 
 */

#pragma once

namespace rhi
{
class CommandBuffer
{
public:
    virtual ~CommandBuffer() = default;

    virtual void* GetNativePtr() const = 0;
};
}   // namespace rhi
