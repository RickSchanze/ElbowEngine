//
// Created by Echo on 24-12-23.
//

#pragma once
#include "Core/Base/CoreTypeDef.h"
#include "Enums.h"

namespace platform::rhi
{
class Image;
}
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

    static void EndSingleTransferCommand(const core::SharedPtr<rhi::CommandBuffer>& command_buffer);

    static void PipelineBarrier(
        rhi::ImageLayout old, rhi::ImageLayout new_, rhi::Image* target, rhi::ImageSubresourceRange range, rhi::AccessFlags src_mask,
        rhi::AccessFlags dst_mask, rhi::PipelineStageFlags src_stage, rhi::PipelineStageFlags dst_stage
    );
};

}   // namespace platform
