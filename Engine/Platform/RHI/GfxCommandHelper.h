//
// Created by Echo on 24-12-23.
//

#pragma once
#include "Core/Base/CoreTypeDef.h"

namespace platform::rhi
{
class CommandBuffer;
}
namespace platform
{

class GfxCommandHelper
{
public:
    static core::SharedPtr<rhi::CommandBuffer> BeginSingleTransferCommand();

    static void EndSingleTransferCommand(rhi::CommandBuffer& command_buffer);
};

}   // namespace platform
