//
// Created by Echo on 2025/4/3.
//

#include "CommandBuffer.hpp"

#include "Commands.hpp"

using namespace NRHI;

void CommandBuffer::ImagePipelineBarrier(const ImageLayout old_layout, const ImageLayout new_layout, Image *target,
                                         const ImageSubresourceRange &subresource_range, const AccessFlags src_access, const AccessFlags dst_access,
                                         const PipelineStageFlags src_stage, const PipelineStageFlags dst_stage)
{
    Enqueue<Cmd_ImagePipelineBarrier>(old_layout, new_layout, target, subresource_range, src_access, dst_access, src_stage, dst_stage);
}

void CommandBuffer::ImagePipelineBarrier(ImageLayout old_layout, ImageLayout new_layout, const SharedPtr<Image> &target,
                                         const ImageSubresourceRange &subresource_range, AccessFlags src_access, AccessFlags dst_access,
                                         PipelineStageFlags src_stage, PipelineStageFlags dst_stage)
{
    Enqueue<Cmd_ImagePipelineBarrier>(old_layout, new_layout, target.get(), subresource_range, src_access, dst_access, src_stage, dst_stage);
}

void CommandBuffer::BeginDebugLabel(const StringView label)
{
#ifdef ELBOW_DEBUG
    Enqueue<Cmd_BeginCommandLabel>(label);
#endif
}

void CommandBuffer::EndDebugLabel()
{
#ifdef ELBOW_DEBUG
    Enqueue<Cmd_EndCommandDebugLabel>();
#endif
}

void CommandBuffer::BeginRender(const Array<RenderAttachment> &colors, const RenderAttachment &depth, Vector2f size)
{
    Enqueue<Cmd_BeginRender>(colors, depth, size);
}

void CommandBuffer::EndRender()
{
    Enqueue<Cmd_EndRender>();
}

void CommandBuffer::SetScissor(const Rect2Df &scissor)
{
    Enqueue<Cmd_SetScissor>(scissor);
}

void CommandBuffer::SetViewport(const Rect2Df &viewport)
{
    Enqueue<Cmd_SetViewport>(viewport);
}

void CommandBuffer::BindPipeline(Pipeline *pipeline)
{
    Enqueue<Cmd_BindPipeline>(pipeline);
}

void CommandBuffer::BindComputePipeline(Pipeline *pipeline)
{
    Enqueue<Cmd_BindComputePipeline>(pipeline);
}

void CommandBuffer::BindDescriptorSet(Pipeline *pipeline, DescriptorSet *set)
{
    Enqueue<Cmd_BindDescriptorSet>(pipeline, set);
}

void CommandBuffer::BindDescriptorSetCompute(Pipeline *pipeline, DescriptorSet *set)
{
    Enqueue<Cmd_BindDescriptorSetCompute>(pipeline, set);
}

void CommandBuffer::BindVertexBuffer(Buffer *buffer, UInt32 offset, UInt32 binding)
{
    Enqueue<Cmd_BindVertexBuffer>(buffer, offset, binding);
}

void CommandBuffer::BindVertexBuffer(const SharedPtr<Buffer> &buffer, UInt32 offset, UInt32 binding)
{
    Enqueue<Cmd_BindVertexBuffer>(buffer.get(), offset, binding);
}

void CommandBuffer::BindIndexBuffer(Buffer *buffer_, const UInt64 offset_)
{
    Enqueue<Cmd_BindIndexBuffer>(buffer_, offset_);
}

void CommandBuffer::BindIndexBuffer(const SharedPtr<Buffer> &buffer, const UInt64 offset_)
{
    Enqueue<Cmd_BindIndexBuffer>(buffer.get(), offset_);
}

void CommandBuffer::DrawIndexed(const uint32_t index_count_, const uint32_t instance_count_, const UInt32 first_index_, const String &debug_name)
{
    Enqueue<Cmd_DrawIndexed>(index_count_, instance_count_, first_index_ DEBUG_ONLY(, debug_name));
}

void CommandBuffer::CopyBufferToImage(Buffer *src, Image *dst, const ImageSubresourceRange &subresource_range, const Vector3i offset,
                                      const Vector3i size)
{
    Enqueue<Cmd_CopyBufferToImage>(src, dst, subresource_range, offset, size);
}

void CommandBuffer::CopyImageToBuffer(Image *src, Buffer *dst, const ImageSubresourceRange &subresource_range, const Vector3i offset,
                                      const Vector3i size)
{
    Enqueue<Cmd_CopyImageToBuffer>(src, dst, subresource_range, offset, size);
}

void CommandBuffer::CopyBuffer(Buffer *src, Buffer *dst, UInt64 size)
{
    Enqueue<Cmd_CopyBuffer>(src, dst, size);
}

void CommandBuffer::BlitImage(Image *src, Image *dst, const ImageBlitDesc &desc)
{
    Enqueue<Cmd_BlitImage>(src, dst, desc);
}

void CommandBuffer::PushConstant(Pipeline *InPipeline, UInt32 InOffset, UInt32 InSize, void *InData, ShaderStage InStage)
{
    Enqueue<Cmd_PushConstant>(InPipeline, InOffset, InSize, InData);
}