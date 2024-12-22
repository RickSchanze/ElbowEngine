//
// Created by Echo on 24-12-21.
//

#pragma once
#include "Core/CoreGlobal.h"
#include "Platform/PlatformLogcat.h"

namespace platform::rhi
{
class Buffer;
}
namespace platform::rhi
{

enum class RHICommandType
{
    CopyBuffer,
};

struct RHICommand
{
    virtual ~RHICommand()                                = default;
    [[nodiscard]] virtual RHICommandType GetType() const = 0;
};

struct Cmd_CopyBuffer final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override { return RHICommandType::CopyBuffer; }

    Buffer* src;
    Buffer* dst;
    size_t  size = 0;

    Cmd_CopyBuffer(Buffer* src_, Buffer* dst_, const size_t size_ = 0) : src(src_), dst(dst_), size(size_)
    {

    }
};

}   // namespace platform::rhi

template <typename T, typename... Args>
T* NewRHICommand(Args&&... args)
{
    return NewDoubleFrameTemp<T>(Forward<Args>(args)...);
}
