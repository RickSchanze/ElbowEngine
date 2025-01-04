//
// Created by Echo on 24-12-22.
//

#include "CommandBuffer_Vulkan.h"

#include "Core/Async/Execution/StartAsync.h"
#include "Core/Async/Execution/Then.h"
#include "Core/Async/ThreadManager.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Profiler/ProfileMacro.h"
#include "GfxContext_Vulkan.h"
#include "Platform/Config/PlatformConfig.h"
#include "Platform/RHI/Commands.h"

#include <meta/meta.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/transform.hpp>

using namespace core::exec;
using namespace platform::rhi::vulkan;
using namespace ranges::views;
using namespace ranges;

CommandPool_Vulkan::CommandPool_Vulkan(CommandPoolCreateInfo info) : CommandPool(info)
{
    const auto&             ctx                                    = *GetVulkanGfxContext();
    VkCommandPoolCreateInfo pool_info                              = {};
    pool_info.sType                                                = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    const auto& [graphics_family, present_family, transfer_family] = ctx.GetCurrentQueueFamilyIndices();
    switch (info.type)
    {
    case QueueFamilyType::Graphics: pool_info.queueFamilyIndex = *graphics_family; break;
    case QueueFamilyType::Compute: throw core::NotSupportException("计算队列尚不支持"); break;
    case QueueFamilyType::Transfer: pool_info.queueFamilyIndex = *transfer_family; break;
    case QueueFamilyType::Present: break;
    }
    if (info.allow_reset)
    {
        pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    }
    command_pool_ = ctx.CreateCommandPool_VK(pool_info);
}

CommandPool_Vulkan::~CommandPool_Vulkan()
{
    const auto& ctx = *GetVulkanGfxContext();
    ctx.DestroyCommandPool_VK(command_pool_);
}

core::SharedPtr<platform::rhi::CommandBuffer> CommandPool_Vulkan::CreateCommandBuffer()
{
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool                 = command_pool_;
    alloc_info.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount          = 1;
    VkCommandBuffer command_buffer;
    const auto&     ctx = *GetVulkanGfxContext();
    ctx.CreateCommandBuffers_VK(alloc_info, &command_buffer);
    return core::MakeShared<CommandBuffer_Vulkan>(command_buffer, command_pool_);
}

core::Array<core::SharedPtr<platform::rhi::CommandBuffer>> CommandPool_Vulkan::CreateCommandBuffers(uint32_t count)
{
    core::Array<VkCommandBuffer> command_buffers;
    command_buffers.resize(count);
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool                 = command_pool_;
    alloc_info.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount          = count;
    const auto& ctx                        = *GetVulkanGfxContext();
    ctx.CreateCommandBuffers_VK(alloc_info, command_buffers.data());
    return command_buffers |
           transform([this](auto& command_buffer) { return core::MakeShared<CommandBuffer_Vulkan>(command_buffer, command_pool_); }) |
           to<core::Array<core::SharedPtr<CommandBuffer>>>();
}

void CommandPool_Vulkan::Reset()
{
    const auto& ctx = *GetVulkanGfxContext();
    vkResetCommandPool(ctx.GetDevice(), command_pool_, 0);
}

CommandBuffer_Vulkan::~CommandBuffer_Vulkan()
{
    const auto& ctx = *GetVulkanGfxContext();
    vkFreeCommandBuffers(ctx.GetDevice(), pool_, 1, &buffer_);
}

void CommandBuffer_Vulkan::Reset()
{
    vkResetCommandBuffer(buffer_, 0);
}

static void ExecuteCmd(VkCommandBuffer cmd, platform::rhi::Cmd_CopyBuffer* cmd_copy_buffer)
{
    PROFILE_SCOPE_AUTO;
    if (cmd_copy_buffer->src && cmd_copy_buffer->dst)
    {
        size_t size = cmd_copy_buffer->size;
        if (size == 0)
        {
            if (cmd_copy_buffer->src->GetSize() > cmd_copy_buffer->dst->GetSize())
            {
                LOGGER.Error(logcat::Platform_RHI_Vulkan, "命令CopyBuffer错误, 源缓冲区大小大于目标缓冲区大小");
                return;
            }
            size = cmd_copy_buffer->src->GetSize();
        }
        if (size > cmd_copy_buffer->dst->GetSize() || size > cmd_copy_buffer->src->GetSize())
        {
            LOGGER.Error(logcat::Platform_RHI_Vulkan, "命令CopyBuffer错误, size大于缓冲区大小");
            return;
        }
        VkBufferCopy copy_region = {};
        copy_region.size         = size;
        vkCmdCopyBuffer(cmd, cmd_copy_buffer->src->GetNativeHandleT<VkBuffer>(), cmd_copy_buffer->dst->GetNativeHandleT<VkBuffer>(), 1, &copy_region);
    }
    else
    {
        LOGGER.Error(logcat::Platform_RHI_Vulkan, "命令CopyBuffer错误 src或者dst为空");
    }
}

AsyncResultHandle<> CommandBuffer_Vulkan::Execute(core::StringView label)
{
    PROFILE_SCOPE_AUTO;
    auto* cfg = core::GetConfig<PlatformConfig>();
    if (cfg->GetEnableMultithreadRender())
    {
        auto scheduler = core::ThreadManager::GetScheduler();
        auto task      = Schedule(scheduler, core::ThreadSlot::Render) | Then([this, label] { InternalExecute(label); });
        return StartAsync(task);
    }
    else
    {
        InternalExecute(label);
        return nullptr;
    }
}

void CommandBuffer_Vulkan::InternalExecute(core::StringView label)
{
    PROFILE_SCOPE_AUTO;
    auto& ctx = *GetVulkanGfxContext();
    if (!recording_)
    {
        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        begin_info.pInheritanceInfo         = nullptr;
        vkBeginCommandBuffer(buffer_, &begin_info);
        recording_ = true;
    }
    VkDebugUtilsLabelEXT label_info = {};
    label_info.sType                = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    label_info.pLabelName           = *label;
    ctx.BeginDebugLabel(buffer_, label_info);
    for (auto& command: commands_)
    {
        switch (command->GetType())
        {
        case RHICommandType::CopyBuffer: ExecuteCmd(buffer_, static_cast<Cmd_CopyBuffer*>(command)); break;
        }
    }
    ctx.EndDebugLabel(buffer_);
    Clear();
}
