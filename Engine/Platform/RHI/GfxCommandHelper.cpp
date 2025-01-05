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
    auto cmd = pool.CreateCommandBuffer(true);
    return cmd;
}

void GfxCommandHelper::EndSingleTransferCommand(CommandBuffer& command_buffer)
{
    auto&           ctx   = GetGfxContextRef();
    const auto      fence = ctx.CreateFence();
    SubmitParameter param{};
    param.fence             = fence.get();
    param.submit_queue_type = QueueFamilyType::Transfer;
    ctx.Submit(command_buffer, param)->Wait();
    fence->SyncWait();
}
