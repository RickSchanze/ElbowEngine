//
// Created by Echo on 24-12-22.
//

#include "CommandBuffer_Vulkan.h"

#include "Core/Async/Execution/StartAsync.h"
#include "Core/Async/Execution/Then.h"
#include "Core/Async/ThreadManager.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Profiler/ProfileMacro.h"
#include "Enums_Vulkan.h"
#include "GfxContext_Vulkan.h"
#include "Platform/Config/PlatformConfig.h"
#include "Platform/PlatformLogcat.h"
#include "Platform/RHI/Commands.h"
#include "Platform/RHI/ImageView.h"

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

core::SharedPtr<platform::rhi::CommandBuffer> CommandPool_Vulkan::CreateCommandBuffer(bool self_managed)
{
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool                 = command_pool_;
    alloc_info.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount          = 1;
    VkCommandBuffer command_buffer;
    const auto&     ctx = *GetVulkanGfxContext();
    ctx.CreateCommandBuffers_VK(alloc_info, &command_buffer);
    return core::MakeShared<CommandBuffer_Vulkan>(command_buffer, command_pool_, self_managed);
}

core::Array<core::SharedPtr<platform::rhi::CommandBuffer>> CommandPool_Vulkan::CreateCommandBuffers(uint32_t count, bool self_managed)
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
    return command_buffers | transform([this, self_managed](auto& command_buffer) {
               return core::MakeShared<CommandBuffer_Vulkan>(command_buffer, command_pool_, self_managed);
           }) |
           to<core::Array<core::SharedPtr<CommandBuffer>>>();
}

void CommandPool_Vulkan::Reset()
{
    const auto& ctx = *GetVulkanGfxContext();
    vkResetCommandPool(ctx.GetDevice(), command_pool_, 0);
}

CommandBuffer_Vulkan::~CommandBuffer_Vulkan()
{
    if (self_managed_)
    {
        const auto& ctx = *GetVulkanGfxContext();
        vkFreeCommandBuffers(ctx.GetDevice(), pool_, 1, &buffer_);
    }
}

void CommandBuffer_Vulkan::Reset()
{
    vkResetCommandBuffer(buffer_, 0);
}

static void ExecuteCmdCopyBuffer(VkCommandBuffer cmd, platform::rhi::Cmd_CopyBuffer* cmd_copy_buffer)
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

static void ExecuteCmdBindPipeline(VkCommandBuffer cmd, platform::rhi::Cmd_BindPipeline* cmd_bind_pipeline)
{
    PROFILE_SCOPE_AUTO;
    if (cmd_bind_pipeline == nullptr || cmd_bind_pipeline->pipeline == nullptr || !cmd_bind_pipeline->pipeline->IsValid())
    {
        LOGGER.Error(logcat::Platform_RHI_Vulkan, "命令BindPipeline错误, 传入的pipeline无效");
        return;
    }
    auto native_pipeline = cmd_bind_pipeline->pipeline->GetNativeHandleT<VkPipeline>();
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, native_pipeline);
}

static void ExecuteCmdBindVertexBuffer(VkCommandBuffer cmd, platform::rhi::Cmd_BindVertexBuffer* cmd_bind_vertex_buffer)
{
    PROFILE_SCOPE_AUTO;
    if (cmd_bind_vertex_buffer == nullptr || cmd_bind_vertex_buffer->buffer == nullptr || !cmd_bind_vertex_buffer->buffer->IsValid())
    {
        LOGGER.Error(logcat::Platform_RHI_Vulkan, "命令BindVertexBuffer错误, 传入的buffer无效");
        return;
    }
    auto native_buffer = cmd_bind_vertex_buffer->buffer->GetNativeHandleT<VkBuffer>();
    vkCmdBindVertexBuffers(cmd, 0, 1, &native_buffer, &cmd_bind_vertex_buffer->offset);
}

static void ExecuteCmdBindIndexBuffer(VkCommandBuffer cmd, platform::rhi::Cmd_BindIndexBuffer* cmd_bind_index_buffer)
{
    PROFILE_SCOPE_AUTO;
    if (cmd_bind_index_buffer == nullptr || cmd_bind_index_buffer->buffer == nullptr || !cmd_bind_index_buffer->buffer->IsValid())
    {
        LOGGER.Error(logcat::Platform_RHI_Vulkan, "命令BindIndexBuffer错误, 传入的buffer无效");
        return;
    }
    auto native_buffer = cmd_bind_index_buffer->buffer->GetNativeHandleT<VkBuffer>();
    vkCmdBindIndexBuffer(cmd, native_buffer, cmd_bind_index_buffer->offset, VK_INDEX_TYPE_UINT32);
}

static void ExecuteCmdDrawIndexed(VkCommandBuffer cmd, platform::rhi::Cmd_DrawIndexed* cmd_draw_indexed)
{
    PROFILE_SCOPE_AUTO;
    if (cmd_draw_indexed == nullptr)
    {
        LOGGER.Error(logcat::Platform_RHI_Vulkan, "命令DrawIndexed错误, 传入的cmd_draw_indexed无效");
        return;
    }
    vkCmdDrawIndexed(cmd, cmd_draw_indexed->index_count, 1, 0, 0, 0);
}

static void ExecuteCmdBeginRender(VkCommandBuffer& cmd, platform::rhi::Cmd_BeginRender* cmd_begin_render_pass)
{
    PROFILE_SCOPE_AUTO;
    if (cmd_begin_render_pass == nullptr)
    {
        LOGGER.Error(logcat::Platform_RHI_Vulkan, "命令BeginRender错误, 传入的cmd_begin_render_pass无效");
        return;
    }
    core::Array<VkRenderingAttachmentInfo> colors;
    for (int i = 0; i < cmd_begin_render_pass->colors.size(); i++)
    {
        auto& color = cmd_begin_render_pass->colors[i];
        if (color.target == nullptr || !color.target->IsValid())
        {
            LOGGER.Error(logcat::Platform_RHI_Vulkan, "命令BeginRender错误, 传入的colors[{}].target无效", i);
            return;
        }
        VkRenderingAttachmentInfo color_info = {};
        color_info.sType                     = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        color_info.imageView                 = color.target->GetNativeHandleT<VkImageView>();
        color_info.imageLayout               = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_info.loadOp                    = RHIAttachmentLoadOpToVkAttachmentLoadOp(color.load_op);
        color_info.storeOp                   = RHIAttachmentStoreOpToVkAttachmentStoreOp(color.store_op);
        VkClearValue clear_value             = {};
        clear_value.color.float32[0]         = color.clear_color.r;
        clear_value.color.float32[1]         = color.clear_color.g;
        clear_value.color.float32[2]         = color.clear_color.b;
        clear_value.color.float32[3]         = color.clear_color.a;
        color_info.clearValue                = clear_value;
        colors.push_back(color_info);
    }
    VkRenderingAttachmentInfo depth_info = {};
    depth_info.sType                     = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    if (cmd_begin_render_pass->depth.target != nullptr && cmd_begin_render_pass->depth.target->IsValid())
    {
        depth_info.imageView           = cmd_begin_render_pass->depth.target->GetNativeHandleT<VkImageView>();
        depth_info.imageLayout         = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth_info.loadOp              = RHIAttachmentLoadOpToVkAttachmentLoadOp(cmd_begin_render_pass->depth.load_op);
        depth_info.storeOp             = RHIAttachmentStoreOpToVkAttachmentStoreOp(cmd_begin_render_pass->depth.store_op);
        VkClearValue clear_value       = {};
        clear_value.depthStencil.depth = cmd_begin_render_pass->depth.clear_color.r;
        depth_info.clearValue          = clear_value;
    }
    VkRenderingInfo render_info      = {};
    render_info.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO;
    render_info.renderArea.offset    = {0, 0};
    render_info.renderArea.extent    = {cmd_begin_render_pass->render_size.width, cmd_begin_render_pass->render_size.height};
    render_info.layerCount           = 1;
    render_info.colorAttachmentCount = static_cast<uint32_t>(colors.size());
    render_info.pColorAttachments    = colors.data();
    render_info.pDepthAttachment     = &depth_info;
    vkCmdBeginRendering(cmd, &render_info);
}

static void ExecuteCmdEndRender(VkCommandBuffer& cmd)
{
    PROFILE_SCOPE_AUTO;
    vkCmdEndRendering(cmd);
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
        empty_ = false;
        switch (command->GetType())
        {
        case RHICommandType::CopyBuffer: ExecuteCmdCopyBuffer(buffer_, static_cast<Cmd_CopyBuffer*>(command)); break;
        case RHICommandType::BindGraphicsPipeline: ExecuteCmdBindPipeline(buffer_, static_cast<Cmd_BindPipeline*>(command)); break;
        case RHICommandType::BindVertexBuffer: ExecuteCmdBindVertexBuffer(buffer_, static_cast<Cmd_BindVertexBuffer*>(command)); break;
        case RHICommandType::BindIndexBuffer: ExecuteCmdBindIndexBuffer(buffer_, static_cast<Cmd_BindIndexBuffer*>(command)); break;
        case RHICommandType::DrawIndexed: ExecuteCmdDrawIndexed(buffer_, static_cast<Cmd_DrawIndexed*>(command)); break;
        case RHICommandType::BeginRender: ExecuteCmdBeginRender(buffer_, static_cast<Cmd_BeginRender*>(command)); break;
        case RHICommandType::EndRender: ExecuteCmdEndRender(buffer_); break;
        }
        Delete(command);
    }
    ctx.EndDebugLabel(buffer_);
    Clear();
}
