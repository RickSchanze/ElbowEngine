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


SharedPtr<NRHI::CommandBuffer> NRHI::GfxCommandHelper::BeginSingleCommand() {
    auto &ctx = GetGfxContextRef();
    auto &pool = ctx.GetTransferPool();
    auto cmd = pool.CreateCommandBuffer(true);
    cmd->Begin();
    return cmd;
}

void NRHI::GfxCommandHelper::EndSingleCommandTransfer(const SharedPtr<CommandBuffer> &command_buffer) {
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

void NRHI::GfxCommandHelper::EndSingleCommandGraphics(const SharedPtr<CommandBuffer> &command_buffer) {
    command_buffer->End();
    auto &ctx = GetGfxContextRef();
    const auto fence = ctx.CreateFence(false);
    SubmitParameter param{};
    param.fence = fence.Get();
    param.submit_queue_type = QueueFamilyType::Graphics;
    auto fuc = ctx.Submit(command_buffer, param);
    fuc.Get();
    fence->SyncWait();
}

void NRHI::GfxCommandHelper::EndSingleCommandCompute(const SharedPtr<CommandBuffer> &command_buffer) {
    command_buffer->End();
    command_buffer->Execute();
    auto &ctx = GetGfxContextRef();
    const auto fence = ctx.CreateFence(false);
    SubmitParameter param{};
    param.fence = fence.Get();
    param.submit_queue_type = QueueFamilyType::Compute;
    auto fuc = ctx.Submit(command_buffer, param);
    fuc.Get();
    fence->SyncWait();
}

void NRHI::GfxCommandHelper::PipelineBarrier(ImageLayout old, ImageLayout new_, Image *target, const ImageSubresourceRange &range,
                                            AccessFlags src_mask, AccessFlags dst_mask, PipelineStageFlags src_stage, PipelineStageFlags dst_stage) {
    auto cmd = BeginSingleCommand();
    cmd->ImagePipelineBarrier(old, new_, target, range, src_mask, dst_mask, src_stage, dst_stage);
    cmd->Execute();
    EndSingleCommandTransfer(cmd);
}

void NRHI::GfxCommandHelper::CopyDataToBuffer(const void *data, Buffer *target, UInt32 size, UInt32 offset) {
    auto &ctx = GetGfxContextRef();
    BufferDesc staging_buffer_info{size, BUB_TransferSrc, BMPB_HostVisible | BMPB_HostCoherent};
    auto staging_buffer = ctx.CreateBuffer(staging_buffer_info);
    void *mapped_data = staging_buffer->BeginWrite();
    Memcpy(mapped_data, data, size);
    staging_buffer->EndWrite();
    auto cmd = BeginSingleCommand();
    cmd->CopyBuffer(staging_buffer.get(), target);
    cmd->Execute();
    EndSingleCommandTransfer(cmd);
}

void NRHI::GfxCommandHelper::CopyDataToImage2D(const void *data, Image *target, UInt32 size, Vector3i offset, Vector3i copy_range) {
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

    PipelineBarrier(ImageLayout::ShaderReadOnly, ImageLayout::TransferDst, target, range, AFB_ShaderRead, AFB_TransferWrite, PSFB_FragmentShader,
                    PSFB_Transfer);
    auto cmd = BeginSingleCommand();
    Vector3i img_size{};
    img_size.X = copy_range.X == 0 ? target->GetWidth() : copy_range.X;
    img_size.Y = copy_range.Y == 0 ? target->GetHeight() : copy_range.Y;
    img_size.Z = copy_range.Z == 0 ? 1 : copy_range.Z;
    cmd->CopyBufferToImage(staging_buffer.get(), target, range, offset, img_size);
    cmd->Execute();
    EndSingleCommandTransfer(cmd);
    PipelineBarrier(ImageLayout::TransferDst, ImageLayout::ShaderReadOnly, target, range, AFB_TransferWrite, AFB_ShaderRead, PSFB_Transfer,
                    PSFB_FragmentShader);
}
