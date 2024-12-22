//
// Created by Echo on 24-12-21.
//

#pragma once
#include "Core/CoreGlobal.h"

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
    [[nodiscard]] virtual RHICommandType GetType() const = 0;
};

struct Cmd_CopyBuffer : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override { return RHICommandType::CopyBuffer; }

    Buffer* src;
    Buffer* dst;

    Cmd_CopyBuffer(Buffer* src, Buffer* dst) : src(src), dst(dst) {}
};

}   // namespace platform::rhi

template<typename T, typename ... Args>
T* NewRHICommand(Args&&... args)
{
    return NewDoubleFrameTemp<T>(Forward<Args>(args)...);
}
