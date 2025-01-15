//
// Created by Echo on 24-12-23.
//

#pragma once
#include "Core/Base/CoreTypeDef.h"
#include "Enums.h"

namespace platform::rhi
{
class Buffer;
class Image;
}   // namespace platform::rhi
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

    /**
     * 将数据拷贝到buffer
     * @param data
     * @param target
     * @param size 大小 等于0 表示整个buffer
     * @param offset 偏移(尚未实现)
     */
    static void CopyDataToBuffer(const void* data, rhi::Buffer* target, uint32_t size = 0, uint32_t offset = 0);

    static void
    CopyDataToImage2D(const void* data, rhi::Image* target, UInt32 size);
};

}   // namespace platform
