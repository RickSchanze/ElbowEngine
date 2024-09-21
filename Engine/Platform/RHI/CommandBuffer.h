/**
 * @file CommandBuffer.h
 * @author Echo 
 * @Date 24-9-21
 * @brief 
 */

#pragma once
#include "PlatformCommon.h"

RHI_NAMESPACE_BEGIN

class CommandBuffer
{
public:
    virtual ~CommandBuffer() = default;

    virtual void* GetNativePtr() const = 0;
};

RHI_NAMESPACE_END
