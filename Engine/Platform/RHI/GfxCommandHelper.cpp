//
// Created by Echo on 2025/3/25.
//
#include "GfxCommandHelper.hpp"

#include "Buffer.hpp"
#include "CommandBuffer.hpp"
#include "Commands.hpp"
#include "Core/Memory/Memory.hpp"
#include "GfxContext.hpp"
#include "Image.hpp"
#include "SyncPrimitives.hpp"


SharedPtr<rhi::CommandBuffer> rhi::GfxCommandHelper::BeginSingleTransferCommand() {
    auto &ctx = GetGfxContextRef();
    auto &pool = ctx.GetTransferPool();
    auto cmd = pool.CreateCommandBuffer(true);
    cmd->Begin();
    return cmd;
}

void rhi::GfxCommandHelper::EndSingleTransferCommand(const SharedPtr<CommandBuffer> &command_buffer) {
    command_buffer->End();
    auto &ctx = GetGfxContextRef();
    const auto fence = ctx.CreateFence(false);
    SubmitParameter param{};
    param.fence = fence.Get();
    param.submit_queue_type = QueueFamilyType::Transfer;
    auto fuc = ctx.Submit(command_buffer, param);
    fuc.Get();
    fence->SyncWait();
}

void rhi::GfxCommandHelper::PipelineBarrier(ImageLayout old, ImageLayout new_, Image *target, ImageSubresourceRange range, AccessFlags src_mask,
                                            AccessFlags dst_mask, PipelineStageFlags src_stage, PipelineStageFlags dst_stage) {
    auto cmd = BeginSingleTransferCommand();
    cmd->Enqueue<Cmd_ImagePipelineBarrier>(old, new_, target, range, src_mask, dst_mask, src_stage, dst_stage);
    cmd->Execute();
    EndSingleTransferCommand(cmd);
}

void rhi::GfxCommandHelper::CopyDataToBuffer(const void *data, Buffer *target, UInt32 size, UInt32 offset) {
    auto &ctx = GetGfxContextRef();
    BufferDesc staging_buffer_info{size, BUB_TransferSrc, BMPB_HostVisible | BMPB_HostCoherent};
    auto staging_buffer = ctx.CreateBuffer(staging_buffer_info);
    void *mapped_data = staging_buffer->BeginWrite();
    Memcpy(mapped_data, data, size);
    staging_buffer->EndWrite();
    auto cmd = BeginSingleTransferCommand();
    cmd->Enqueue<Cmd_CopyBuffer>(staging_buffer.get(), target);
    cmd->Execute();
    EndSingleTransferCommand(cmd);
}

void rhi::GfxCommandHelper::CopyDataToImage2D(const void *data, Image *target, UInt32 size, Vector3i offset, Vector3i copy_range) {
    auto &ctx = GetGfxContextRef();
    BufferDesc staging_buffer_info{size, BUB_TransferSrc, BMPB_HostVisible | BMPB_HostCoherent};
    auto staging_buffer = ctx.CreateBuffer(staging_buffer_info);
    void *mapped_data = staging_buffer->BeginWrite();
    Memcpy(mapped_data, data, size);
    staging_buffer->EndWrite();
    // 执行图像布局变换
    ImageSubresourceRange range{};
    range.aspect_mask = IA_Color;
    range.base_mip_level = 0;
    range.level_count = 1;
    range.base_array_layer = 0;
    range.layer_count = 1;

    PipelineBarrier(ImageLayout::Undefined, ImageLayout::TransferDst, target, range, 0, AFB_TransferWrite, PSFB_TopOfPipe, PSFB_Transfer);
    auto cmd = BeginSingleTransferCommand();
    Vector3i img_size{};
    img_size.x = copy_range.x == 0 ? target->GetWidth() : copy_range.x;
    img_size.y = copy_range.y == 0 ? target->GetHeight() : copy_range.y;
    img_size.z = copy_range.z == 0 ? 1 : copy_range.z;
    cmd->Enqueue<Cmd_CopyBufferToImage>(staging_buffer.get(), target, range, offset, img_size);
    cmd->Execute();
    EndSingleTransferCommand(cmd);
    PipelineBarrier(ImageLayout::TransferDst, ImageLayout::ShaderReadOnly, target, range, AFB_TransferWrite, AFB_ShaderRead, PSFB_Transfer,
                    PSFB_FragmentShader);
}
