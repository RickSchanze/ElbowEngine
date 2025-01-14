//
// Created by Echo on 24-12-23.
//

#include "GfxCommandHelper.h"

#include "CommandBuffer.h"
#include "Commands.h"
#include "GfxContext.h"
#include "SyncPrimitives.h"

using namespace platform;
using namespace platform::rhi;

core::SharedPtr<CommandBuffer> GfxCommandHelper::BeginSingleTransferCommand()
{
    auto& ctx  = GetGfxContextRef();
    auto& pool = ctx.GetTransferPool();
    auto  cmd  = pool.CreateCommandBuffer(true);
    cmd->Begin();
    return cmd;
}

void GfxCommandHelper::EndSingleTransferCommand(const core::SharedPtr<CommandBuffer>& command_buffer)
{
    command_buffer->End();
    auto&           ctx   = GetGfxContextRef();
    const auto      fence = ctx.CreateFence(false);
    SubmitParameter param{};
    param.fence             = fence.Get();
    param.submit_queue_type = QueueFamilyType::Transfer;
    ctx.Submit(command_buffer, param)->Wait();
    fence->SyncWait();
}

void GfxCommandHelper::PipelineBarrier(
    rhi::ImageLayout old, rhi::ImageLayout new_, rhi::Image* target, rhi::ImageSubresourceRange range, rhi::AccessFlags src_mask,
    rhi::AccessFlags dst_mask, rhi::PipelineStageFlags src_stage, rhi::PipelineStageFlags dst_stage
)
{
    auto cmd = BeginSingleTransferCommand();
    cmd->Enqueue<Cmd_ImagePipelineBarrier>(old, new_, target, range, src_mask, dst_mask, src_stage, dst_stage);
    cmd->Execute("");
    EndSingleTransferCommand(cmd);
}
