//
// Created by Echo on 24-12-22.
//

#include "CommandBuffer_Vulkan.h"

#include "GfxContext_Vulkan.h"

#include <meta/meta.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/utility/addressof.hpp>
#include <range/v3/view/transform.hpp>

platform::rhi::vulkan::CommandPool_Vulkan::CommandPool_Vulkan(CommandPoolCreateInfo info) : CommandPool(info)
{
    const auto&             ctx                   = *GetVulkanGfxContext();
    VkCommandPoolCreateInfo pool_info             = {};
    pool_info.sType                               = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    const auto& [graphics_family, present_family] = ctx.GetCurrentQueueFamilyIndices();
    switch (info.type)
    {
    case CommandPoolType::Graphics: pool_info.queueFamilyIndex = *graphics_family; break;
    case CommandPoolType::Compute: throw core::NotSupportException("计算队列尚不支持"); break;
    case CommandPoolType::Transfer: throw core::NotSupportException("传输队列尚不支持"); break;
    }
    if (info.allow_reset)
    {
        pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    }
    command_pool_ = ctx.CreateCommandPool(pool_info);
}

platform::rhi::vulkan::CommandPool_Vulkan::~CommandPool_Vulkan()
{
    const auto& ctx = *GetVulkanGfxContext();
    ctx.DestroyCommandPool(command_pool_);
}

core::SharedPtr<platform::rhi::CommandBuffer> platform::rhi::vulkan::CommandPool_Vulkan::CreateCommandBuffer()
{
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool                 = command_pool_;
    alloc_info.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount          = 1;
    VkCommandBuffer command_buffer;
    const auto&     ctx = *GetVulkanGfxContext();
    ctx.CreateCommandBuffers(alloc_info, &command_buffer);
    return core::MakeShared<CommandBuffer_Vulkan>(command_buffer);
}

core::Array<core::SharedPtr<platform::rhi::CommandBuffer>> platform::rhi::vulkan::CommandPool_Vulkan::CreateCommandBuffers(uint32_t count)
{
    core::Array<VkCommandBuffer> command_buffers;
    command_buffers.resize(count);
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool                 = command_pool_;
    alloc_info.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount          = count;
    const auto& ctx                        = *GetVulkanGfxContext();
    ctx.CreateCommandBuffers(alloc_info, command_buffers.data());
    return command_buffers | ranges::views::transform([](auto& command_buffer) { return core::MakeShared<CommandBuffer_Vulkan>(command_buffer); }) |
           ranges::to<core::Array<core::SharedPtr<CommandBuffer>>>();
}

void platform::rhi::vulkan::CommandBuffer_Vulkan::BeginRecord()
{
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(buffer_, &begin_info);
}

void platform::rhi::vulkan::CommandBuffer_Vulkan::EndRecord()
{
    vkEndCommandBuffer(buffer_);
}

void platform::rhi::vulkan::CommandBuffer_Vulkan::EnqueueCommand(RHICommand* command) {}

void platform::rhi::vulkan::CommandBuffer_Vulkan::Reset()
{
    vkResetCommandBuffer(buffer_, 0);
}
