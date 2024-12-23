//
// Created by Echo on 24-12-23.
//

#include "GfxCommandHelper.h"

#include "CommandBuffer.h"
#include "GfxContext.h"
#include "SyncPrimitives.h"

using namespace platform;
using namespace platform::rhi;

core::SharedPtr<CommandBuffer> GfxCommandHelper::BeginSingleTransferCommand()
{
    auto& ctx  = GetGfxContextRef();
    auto& pool = ctx.GetTransferPool();
    return pool.CreateCommandBuffer();
}

void GfxCommandHelper::EndSingleTransferCommand(const CommandBuffer& command_buffer)
{
    auto&           ctx   = GetGfxContextRef();
    const auto      fence = ctx.CreateFence();
    SubmitParameter param{};
    param.fence             = fence.get();
    param.submit_queue_type = QueueFamilyType::Transfer;
    core::exec::SyncWait(ctx.Submit(command_buffer, param));
    fence->SyncWait();
}
