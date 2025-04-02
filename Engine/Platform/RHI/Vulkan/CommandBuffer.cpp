//
// Created by Echo on 2025/3/25.
//
#include "CommandBuffer.hpp"

#include "Core/Async/Exec/Just.hpp"
#include "Core/Async/ThreadManager.hpp"
#include "Core/Config/ConfigManager.hpp"
#include "Core/Math/Math.hpp"
#include "Core/Profile.hpp"
#include "Enums.hpp"
#include "GfxContext.hpp"
#include "Pipeline.hpp"
#include "Platform/Config/PlatformConfig.hpp"
#include "Platform/RHI/Buffer.hpp"
#include "Platform/RHI/Commands.hpp"
#include "Platform/RHI/DescriptorSet.hpp"
#include "Platform/RHI/ImageView.hpp"

using namespace exec;
using namespace rhi;

static void ExecuteCmdBindDescriptorSet(const VkCommandBuffer cmd, const Cmd_BindDescriptorSet *cmd_bind_descriptor_set) {
    if (cmd_bind_descriptor_set == nullptr || cmd_bind_descriptor_set->set == nullptr || !cmd_bind_descriptor_set->set->IsValid()) {
        Log(Error) << "命令BindDescriptorSet错误, 传入的cmd_bind_descriptor_set无效";
        return;
    }
    const auto pipeline = static_cast<GraphicsPipeline_Vulkan *>(cmd_bind_descriptor_set->pipeline);
    const VkPipelineLayout layout = pipeline->GetPipelineLayout();
    const auto set = cmd_bind_descriptor_set->set->GetNativeHandleT<VkDescriptorSet>();
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &set, 0, nullptr);
}

static void ExecuteCmdCopyBuffer(const VkCommandBuffer cmd, const Cmd_CopyBuffer *cmd_copy_buffer) {
    ProfileScope _(__func__);
    if (cmd_copy_buffer->src && cmd_copy_buffer->dst) {
        size_t size = cmd_copy_buffer->size;
        if (size == 0) {
            if (cmd_copy_buffer->src->GetSize() > cmd_copy_buffer->dst->GetSize()) {
                Log(Error) << "命令CopyBuffer错误, 源缓冲区大小大于目标缓冲区大小";
                return;
            }
            size = cmd_copy_buffer->src->GetSize();
        }
        if (size > cmd_copy_buffer->dst->GetSize() || size > cmd_copy_buffer->src->GetSize()) {
            Log(Error) << "命令CopyBuffer错误, size大于缓冲区大小";
            return;
        }
        VkBufferCopy copy_region = {};
        copy_region.size = size;
        vkCmdCopyBuffer(cmd, cmd_copy_buffer->src->GetNativeHandleT<VkBuffer>(), cmd_copy_buffer->dst->GetNativeHandleT<VkBuffer>(), 1, &copy_region);
    } else {
        Log(Error) << "命令CopyBuffer错误, src或者dst为空";
    }
}

static void ExecuteCmdBindPipeline(const VkCommandBuffer cmd, const Cmd_BindPipeline *cmd_bind_pipeline) {
    ProfileScope _(__func__);
    if (cmd_bind_pipeline == nullptr || cmd_bind_pipeline->pipeline == nullptr || !cmd_bind_pipeline->pipeline->IsValid()) {
        Log(Error) << "命令BindPipeline错误, 传入的pipeline无效";
        return;
    }
    const auto native_pipeline = cmd_bind_pipeline->pipeline->GetNativeHandleT<VkPipeline>();
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, native_pipeline);
}

static void ExecuteCmdBindVertexBuffer(const VkCommandBuffer cmd, const Cmd_BindVertexBuffer *cmd_bind_vertex_buffer) {
    ProfileScope _(__func__);
    if (cmd_bind_vertex_buffer == nullptr || cmd_bind_vertex_buffer->buffer == nullptr || !cmd_bind_vertex_buffer->buffer->IsValid()) {
        Log(Error) << "命令BindVertexBuffer错误, 传入的buffer无效";
        return;
    }
    const auto native_buffer = cmd_bind_vertex_buffer->buffer->GetNativeHandleT<VkBuffer>();
    vkCmdBindVertexBuffers(cmd, cmd_bind_vertex_buffer->binding, 1, &native_buffer, &cmd_bind_vertex_buffer->offset);
}

static void ExecuteCmdBindIndexBuffer(const VkCommandBuffer cmd, const Cmd_BindIndexBuffer *cmd_bind_index_buffer) {
    ProfileScope _(__func__);
    if (cmd_bind_index_buffer == nullptr || cmd_bind_index_buffer->buffer == nullptr || !cmd_bind_index_buffer->buffer->IsValid()) {
        Log(Error) << "命令BindIndexBuffer错误, 传入的buffer无效";
        return;
    }
    const auto native_buffer = cmd_bind_index_buffer->buffer->GetNativeHandleT<VkBuffer>();
    vkCmdBindIndexBuffer(cmd, native_buffer, cmd_bind_index_buffer->offset, VK_INDEX_TYPE_UINT32);
}

static void ExecuteCmdDrawIndexed(const VkCommandBuffer cmd, const Cmd_DrawIndexed *cmd_draw_indexed) {
    ProfileScope _(__func__);
    if (cmd_draw_indexed == nullptr) {
        Log(Error) << "命令DrawIndexed错误, 传入的cmd_draw_indexed无效";
        return;
    }
    vkCmdDrawIndexed(cmd, cmd_draw_indexed->index_count, cmd_draw_indexed->instance_count, cmd_draw_indexed->first_index, 0, 0);
}

static void ExecuteCmdBeginRender(const VkCommandBuffer &cmd, Cmd_BeginRender *cmd_begin_render_pass) {
    ProfileScope _(__func__);
    if (cmd_begin_render_pass == nullptr) {
        Log(Error) << "命令BeginRender错误, 传入的cmd_begin_render_pass无效";
        return;
    }
    Array<VkRenderingAttachmentInfo> colors;
    for (int i = 0; i < cmd_begin_render_pass->colors.Count(); i++) {
        const auto &color = cmd_begin_render_pass->colors[i];
        if (color.target == nullptr || !color.target->IsValid()) {
            Log(Error) << String::Format("命令BeginRender错误, 传入的colors[{}].target无效", i);
            return;
        }
        VkRenderingAttachmentInfo color_info = {};
        color_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        color_info.imageView = color.target->GetNativeHandleT<VkImageView>();
        color_info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_info.loadOp = RHIAttachmentLoadOpToVkAttachmentLoadOp(color.load_op);
        color_info.storeOp = RHIAttachmentStoreOpToVkAttachmentStoreOp(color.store_op);
        VkClearValue clear_value = {};
        clear_value.color.float32[0] = color.clear_color.r;
        clear_value.color.float32[1] = color.clear_color.g;
        clear_value.color.float32[2] = color.clear_color.b;
        clear_value.color.float32[3] = color.clear_color.a;
        color_info.clearValue = clear_value;
        colors.Add(color_info);
    }
    VkRenderingAttachmentInfo depth_info = {};
    depth_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    if (cmd_begin_render_pass->depth.target != nullptr && cmd_begin_render_pass->depth.target->IsValid()) {
        depth_info.imageView = cmd_begin_render_pass->depth.target->GetNativeHandleT<VkImageView>();
        depth_info.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth_info.loadOp = RHIAttachmentLoadOpToVkAttachmentLoadOp(cmd_begin_render_pass->depth.load_op);
        depth_info.storeOp = RHIAttachmentStoreOpToVkAttachmentStoreOp(cmd_begin_render_pass->depth.store_op);
        VkClearValue clear_value = {};
        clear_value.depthStencil.depth = cmd_begin_render_pass->depth.clear_color.r;
        depth_info.clearValue = clear_value;
    }
    VkRenderingInfo render_info = {};
    render_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    render_info.renderArea.offset = {0, 0};
    render_info.renderArea.extent = {static_cast<UInt32>(cmd_begin_render_pass->render_size.x),
                                     static_cast<UInt32>(cmd_begin_render_pass->render_size.y)};
    render_info.layerCount = 1;
    render_info.colorAttachmentCount = static_cast<uint32_t>(colors.Count());
    render_info.pColorAttachments = colors.Data();
    render_info.pDepthAttachment = &depth_info;
    vkCmdBeginRendering(cmd, &render_info);
}

static void ExecuteCmdEndRender(const VkCommandBuffer cmd) {
    ProfileScope _(__func__);
    vkCmdEndRendering(cmd);
}

static void ExecuteCmdImagePipelineBarrier(const VkCommandBuffer cmd, const Cmd_ImagePipelineBarrier *cmd_image_pipeline_barrier) {
    ProfileScope _(__func__);
    if (cmd_image_pipeline_barrier == nullptr || cmd_image_pipeline_barrier->target == nullptr || !cmd_image_pipeline_barrier->target->IsValid()) {
        Log(Error) << "命令ImagePipelineBarrier错误, 传入的target无效";
        return;
    }
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = cmd_image_pipeline_barrier->target->GetNativeHandleT<VkImage>();
    barrier.oldLayout = RHIImageLayoutToVkImageLayout(cmd_image_pipeline_barrier->old_layout);
    barrier.newLayout = RHIImageLayoutToVkImageLayout(cmd_image_pipeline_barrier->new_layout);
    barrier.srcAccessMask = RHIAccessFlagToVkAccessFlag(cmd_image_pipeline_barrier->src_access);
    barrier.dstAccessMask = RHIAccessFlagToVkAccessFlag(cmd_image_pipeline_barrier->dst_access);
    barrier.subresourceRange.aspectMask = RHIImageAspectToVkImageAspect(cmd_image_pipeline_barrier->subresource_range.aspect_mask);
    barrier.subresourceRange.baseMipLevel = cmd_image_pipeline_barrier->subresource_range.base_mip_level;
    barrier.subresourceRange.levelCount = cmd_image_pipeline_barrier->subresource_range.level_count;
    barrier.subresourceRange.baseArrayLayer = cmd_image_pipeline_barrier->subresource_range.base_array_layer;
    barrier.subresourceRange.layerCount = cmd_image_pipeline_barrier->subresource_range.layer_count;

    const VkPipelineStageFlags src_stage = RHIPipelineStageToVkPipelineStage(cmd_image_pipeline_barrier->src_stage);
    const VkPipelineStageFlags dst_stage = RHIPipelineStageToVkPipelineStage(cmd_image_pipeline_barrier->dst_stage);
    vkCmdPipelineBarrier(cmd, src_stage, dst_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

static void ExecuteCmdSetScissor(const VkCommandBuffer cmd, const Cmd_SetScissor *cmd_set_scissor) {
    if (cmd_set_scissor == nullptr || Math::ApproximatelyEqual(cmd_set_scissor->scissor.Area(), 0)) {
        Log(Error) << "命令SetScissor错误, 传入的cmd_set_scissor无效";
        return;
    }
    VkRect2D scissor = {};
    scissor.offset.x = cmd_set_scissor->scissor.pos.x;
    scissor.offset.y = cmd_set_scissor->scissor.pos.y;
    scissor.extent.width = cmd_set_scissor->scissor.size.x;
    scissor.extent.height = cmd_set_scissor->scissor.size.y;
    vkCmdSetScissor(cmd, 0, 1, &scissor);
}

static void ExecuteCmdSetViewport(const VkCommandBuffer cmd, const Cmd_SetViewport *cmd_set_viewport) {
    if (cmd_set_viewport == nullptr || Math::ApproximatelyEqual(cmd_set_viewport->viewport.Area(), 0)) {
        Log(Error) << "命令SetViewport错误, 传入的cmd_set_viewport无效";
        return;
    }
    VkViewport viewport = {};
    viewport.x = cmd_set_viewport->viewport.pos.x;
    viewport.y = cmd_set_viewport->viewport.pos.y;
    viewport.width = cmd_set_viewport->viewport.size.x;
    viewport.height = cmd_set_viewport->viewport.size.y;
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;
    vkCmdSetViewport(cmd, 0, 1, &viewport);
}

static void ExecuteCmdCopyBufferToImage(const VkCommandBuffer cmd, const Cmd_CopyBufferToImage *cmd_copy_buffer_to_image) {
    if (cmd_copy_buffer_to_image == nullptr || cmd_copy_buffer_to_image->src == nullptr || !cmd_copy_buffer_to_image->src->IsValid() ||
        cmd_copy_buffer_to_image->dst == nullptr || !cmd_copy_buffer_to_image->dst->IsValid()) {
        Log(Error) << "命令CopyBufferToImage错误, 传入的cmd_copy_buffer_to_image无效";
        return;
    }
    VkBufferImageCopy copy_region = {};
    copy_region.bufferOffset = 0;
    copy_region.bufferRowLength = 0;
    copy_region.bufferImageHeight = 0;
    copy_region.imageSubresource.aspectMask = RHIImageAspectToVkImageAspect(cmd_copy_buffer_to_image->subresource_range.aspect_mask);
    copy_region.imageSubresource.mipLevel = cmd_copy_buffer_to_image->subresource_range.base_mip_level;
    copy_region.imageSubresource.baseArrayLayer = cmd_copy_buffer_to_image->subresource_range.base_array_layer;
    copy_region.imageSubresource.layerCount = cmd_copy_buffer_to_image->subresource_range.layer_count;
    copy_region.imageOffset = {cmd_copy_buffer_to_image->offset.x, cmd_copy_buffer_to_image->offset.y, cmd_copy_buffer_to_image->offset.z};
    VkExtent3D extent{};
    extent.width = cmd_copy_buffer_to_image->size.x;
    extent.height = cmd_copy_buffer_to_image->size.y;
    extent.depth = cmd_copy_buffer_to_image->size.z;
    copy_region.imageExtent = extent;
    const auto src = cmd_copy_buffer_to_image->src->GetNativeHandleT<VkBuffer>();
    const auto dst = cmd_copy_buffer_to_image->dst->GetNativeHandleT<VkImage>();
    vkCmdCopyBufferToImage(cmd, src, dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_region);
}

static void ExecuteCmdCopyImageToBuffer(const VkCommandBuffer cmd, const Cmd_CopyImageToBuffer *cmd_copy_image_to_buffer) {
    if (cmd_copy_image_to_buffer == nullptr || cmd_copy_image_to_buffer->src == nullptr || !cmd_copy_image_to_buffer->src->IsValid() ||
        cmd_copy_image_to_buffer->dst == nullptr || !cmd_copy_image_to_buffer->dst->IsValid()) {
        Log(Error) << "命令CopyImageToBuffer错误, 传入的cmd_copy_image_to_buffer无效";
        return;
    }
    VkBufferImageCopy copy_region = {};
    copy_region.bufferOffset = 0;
    copy_region.bufferRowLength = 0;
    copy_region.bufferImageHeight = 0;
    copy_region.imageSubresource.aspectMask = RHIImageAspectToVkImageAspect(cmd_copy_image_to_buffer->subresource_range.aspect_mask);
    copy_region.imageSubresource.mipLevel = cmd_copy_image_to_buffer->subresource_range.base_mip_level;
    copy_region.imageSubresource.baseArrayLayer = cmd_copy_image_to_buffer->subresource_range.base_array_layer;
    copy_region.imageSubresource.layerCount = cmd_copy_image_to_buffer->subresource_range.layer_count;
    copy_region.imageOffset = {cmd_copy_image_to_buffer->offset.x, cmd_copy_image_to_buffer->offset.y, cmd_copy_image_to_buffer->offset.z};
    VkExtent3D extent{};
    extent.width = cmd_copy_image_to_buffer->size.x;
    extent.height = cmd_copy_image_to_buffer->size.y;
    extent.depth = cmd_copy_image_to_buffer->size.z;
    copy_region.imageExtent = extent;
    const auto src = cmd_copy_image_to_buffer->src->GetNativeHandleT<VkImage>();
    const auto dst = cmd_copy_image_to_buffer->dst->GetNativeHandleT<VkBuffer>();
    vkCmdCopyImageToBuffer(cmd, src, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst, 1, &copy_region);
}

static void ExecuteCmdBeginCommandDebugLabel(VkCommandBuffer buffer, Cmd_BeginCommandLabel *command) {
    auto &ctx = *GetVulkanGfxContext();
    VkDebugUtilsLabelEXT info = {};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    info.pLabelName = *command->label;
    ctx.BeginDebugLabel(buffer, info);
}

static void ExecuteCmdEndCommandDebugLabel(VkCommandBuffer buffer) {
    auto &ctx = *GetVulkanGfxContext();
    ctx.EndDebugLabel(buffer);
}

static void InternalExecute(VkCommandBuffer buffer, const Array<RHICommand *> &commands) {
    ProfileScope _(__func__);
    // 注意这里不对Command调用delete因为它使用双帧分配器 会自动回收
    // 如果调用了Delete会崩溃
    for (Int32 i = 0; i < commands.Count(); i++) {
        auto &command = commands[i];
        switch (command->GetType()) {
            case RHICommandType::CopyBuffer:
                ExecuteCmdCopyBuffer(buffer, static_cast<Cmd_CopyBuffer *>(command));
                break;
            case RHICommandType::BindGraphicsPipeline:
                ExecuteCmdBindPipeline(buffer, static_cast<Cmd_BindPipeline *>(command));
                break;
            case RHICommandType::BindVertexBuffer:
                ExecuteCmdBindVertexBuffer(buffer, static_cast<Cmd_BindVertexBuffer *>(command));
                break;
            case RHICommandType::BindIndexBuffer:
                ExecuteCmdBindIndexBuffer(buffer, static_cast<Cmd_BindIndexBuffer *>(command));
                break;
            case RHICommandType::DrawIndexed:
                ExecuteCmdDrawIndexed(buffer, static_cast<Cmd_DrawIndexed *>(command));
                break;
            case RHICommandType::BeginRender:
                ExecuteCmdBeginRender(buffer, static_cast<Cmd_BeginRender *>(command));
                break;
            case RHICommandType::EndRender:
                ExecuteCmdEndRender(buffer);
                break;
            case RHICommandType::ImagePipelineBarrier:
                ExecuteCmdImagePipelineBarrier(buffer, static_cast<Cmd_ImagePipelineBarrier *>(command));
                break;
            case RHICommandType::SetScissor:
                ExecuteCmdSetScissor(buffer, static_cast<Cmd_SetScissor *>(command));
                break;
            case RHICommandType::SetViewport:
                ExecuteCmdSetViewport(buffer, static_cast<Cmd_SetViewport *>(command));
                break;
            case RHICommandType::BindDescriptorSet:
                ExecuteCmdBindDescriptorSet(buffer, static_cast<Cmd_BindDescriptorSet *>(command));
                break;
            case RHICommandType::CopyBufferToImage:
                ExecuteCmdCopyBufferToImage(buffer, static_cast<Cmd_CopyBufferToImage *>(command));
                break;
            case RHICommandType::CopyImageToBuffer:
                ExecuteCmdCopyImageToBuffer(buffer, static_cast<Cmd_CopyImageToBuffer *>(command));
                break;
            case RHICommandType::BeginCommandDebugLabel:
                ExecuteCmdBeginCommandDebugLabel(buffer, static_cast<Cmd_BeginCommandLabel *>(command));
                break;
            case RHICommandType::EndCommandDebugLabel:
                ExecuteCmdEndCommandDebugLabel(buffer);
                break;
            default:
                Log(Fatal) << "Unknown RHI Command";
        }
        std::destroy_at(command);
    }
}


CommandPool_Vulkan::CommandPool_Vulkan(CommandPoolCreateInfo info) : CommandPool(info) {
    const auto &ctx = *GetVulkanGfxContext();
    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    const auto &[graphics_family, present_family, transfer_family] = ctx.GetCurrentQueueFamilyIndices();
    switch (info.type) {
        case QueueFamilyType::Graphics:
            pool_info.queueFamilyIndex = *graphics_family;
            break;
        case QueueFamilyType::Compute:
            Log(Fatal) << "计算队列尚不支持";
            break;
        case QueueFamilyType::Transfer:
            pool_info.queueFamilyIndex = *transfer_family;
            break;
        case QueueFamilyType::Present:
            break;
    }
    if (info.allow_reset) {
        pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    }
    command_pool_ = ctx.CreateCommandPool_VK(pool_info);
}

CommandPool_Vulkan::~CommandPool_Vulkan() {
    const auto &ctx = *GetVulkanGfxContext();
    ctx.DestroyCommandPool_VK(command_pool_);
}

CommandBuffer_Vulkan::~CommandBuffer_Vulkan() {
    if (self_managed_) {
        pool_->FreeCommandBuffer(buffer_);
    }
}

ExecFuture<> CommandBuffer_Vulkan::Execute() {
    ProfileScope _(__func__);
    const auto *cfg = GetConfig<PlatformConfig>();
    if (cfg->GetEnableMultithreadRender()) {
        auto task = Just() | Then([commands = commands_, buffer = buffer_] { InternalExecute(buffer, commands); });
        commands_.Clear();
        return ThreadManager::ScheduleFutureAsync(task, NamedThread::Render);
    } else {
        InternalExecute(buffer_, commands_);
        commands_.Clear();
        return MakeExecFuture();
    }
}

void CommandBuffer_Vulkan::Begin() {
    ProfileScope _(__func__);
    const auto *cfg = GetConfig<PlatformConfig>();
    if (cfg->GetEnableMultithreadRender()) {
        auto task = Just() | Then([buffer = buffer_] {
                        VkCommandBufferBeginInfo begin_info = {};
                        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
                        vkBeginCommandBuffer(buffer, &begin_info);
                    });
        ThreadManager::ScheduleFutureAsync(task, NamedThread::Render);
    } else {
        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(buffer_, &begin_info);
    }
}

void CommandBuffer_Vulkan::End() {
    ProfileScope _(__func__);
    const auto *cfg = GetConfig<PlatformConfig>();
    if (cfg->GetEnableMultithreadRender()) {
        auto task = Just() | Then([buffer = buffer_] { vkEndCommandBuffer(buffer); });
        ThreadManager::ScheduleFutureAsync(task, NamedThread::Render);
    } else {
        vkEndCommandBuffer(buffer_);
    }
}

void CommandBuffer_Vulkan::InternalBeginDebugLabel(StringView label) { Enqueue<Cmd_BeginCommandLabel>(label); }

void CommandBuffer_Vulkan::InternalEndDebugLabel() { Enqueue<Cmd_EndCommandDebugLabel>(); }

SharedPtr<CommandBuffer> CommandPool_Vulkan::CreateCommandBuffer(bool self_managed) {
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = command_pool_;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;
    VkCommandBuffer command_buffer;
    const auto &ctx = *GetVulkanGfxContext();
    ctx.CreateCommandBuffers_VK(alloc_info, &command_buffer);
    return MakeShared<CommandBuffer_Vulkan>(command_buffer, this, self_managed);
}

Array<SharedPtr<CommandBuffer>> CommandPool_Vulkan::CreateCommandBuffers(uint32_t count, bool self_managed) {
    Array<VkCommandBuffer> command_buffers;
    command_buffers.Resize(count);
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = command_pool_;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = count;
    const auto &ctx = *GetVulkanGfxContext();
    ctx.CreateCommandBuffers_VK(alloc_info, command_buffers.Data());
    Array<SharedPtr<CommandBuffer>> rtn;
    for (auto &command_buffer: command_buffers) {
        rtn.Add(MakeShared<CommandBuffer_Vulkan>(command_buffer, this, self_managed));
    }
    return rtn;
}

void CommandPool_Vulkan::FreeCommandBuffer(VkCommandBuffer buffer) {
    const auto &ctx = *GetVulkanGfxContext();
    if (buffer == VK_NULL_HANDLE)
        return;
    auto device = ctx.GetDevice();
    const auto *cfg = GetConfig<PlatformConfig>();
    if (cfg->GetEnableMultithreadRender()) {
        auto task = Just() | Then([buffer, device, command_pool = command_pool_] {
                        const VkCommandBuffer b = buffer;
                        vkFreeCommandBuffers(device, command_pool, 1, &b);
                    });
        ThreadManager::ScheduleFutureAsync(task, NamedThread::Render);
    } else {
        vkFreeCommandBuffers(ctx.GetDevice(), command_pool_, 1, &buffer);
    }
}

void CommandPool_Vulkan::Reset() {
    const auto &ctx = *GetVulkanGfxContext();
    vkResetCommandPool(ctx.GetDevice(), command_pool_, 0);
}
