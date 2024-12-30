//
// Created by Echo on 24-12-28.
//

#include "RenderPass_Vulkan.h"

#include "Enums_Vulkan.h"
#include "GfxContext_Vulkan.h"
#include "Platform/PlatformLogcat.h"

using namespace platform::rhi::vulkan;
using namespace platform::rhi;
using namespace core;

struct MySubpassDesc
{
    Array<VkAttachmentReference> color_attachments;
    VkAttachmentReference        depth_attachment{.attachment = UINT32_MAX, .layout = VK_IMAGE_LAYOUT_UNDEFINED};
    bool                         has_depth = false;
};

RenderPass_Vulkan::RenderPass_Vulkan(const RenderPassCreateInfo& info) : RenderPass(info)
{
    auto& ctx    = *GetVulkanGfxContext();
    auto  device = ctx.GetDevice();

    Array<VkAttachmentDescription> descriptions;
    descriptions.resize(info.attachments.size());
    for (size_t i = 0; i < info.attachments.size(); ++i)
    {
        const auto& attachment         = info.attachments[i];
        descriptions[i].format         = RHIFormatToVkFormat(attachment.format);
        descriptions[i].samples        = RHISampleCountToVkSampleCount(attachment.sample_count);
        descriptions[i].loadOp         = RHIAttachmentLoadOpToVkAttachmentLoadOp(attachment.load_op);
        descriptions[i].storeOp        = RHIAttachmentStoreOpToVkAttachmentStoreOp(attachment.store_op);
        descriptions[i].stencilLoadOp  = RHIAttachmentLoadOpToVkAttachmentLoadOp(attachment.stencil_load_op);
        descriptions[i].stencilStoreOp = RHIAttachmentStoreOpToVkAttachmentStoreOp(attachment.stencil_store_op);
        descriptions[i].initialLayout  = RHIImageLayoutToVkImageLayout(attachment.initial_layout);
        descriptions[i].finalLayout    = RHIImageLayoutToVkImageLayout(attachment.final_layout);
    }

    Array<MySubpassDesc> subpass_descs;
    subpass_descs.resize(info.subpasses.size());
    for (size_t i = 0; i < info.subpasses.size(); ++i)
    {
        const auto& subpass = info.subpasses[i];
        subpass_descs[i].color_attachments.resize(subpass.color_attachments.size());
        for (size_t j = 0; j < subpass.color_attachments.size(); ++j)
        {
            const auto& attachment                           = subpass.color_attachments[j];
            subpass_descs[i].color_attachments[j].attachment = attachment.index;
            subpass_descs[i].color_attachments[j].layout     = RHIImageLayoutToVkImageLayout(attachment.render_layout);
        }
        subpass_descs[i].depth_attachment = {
            subpass.depth_attachment.index,                                         //
            RHIImageLayoutToVkImageLayout(subpass.depth_attachment.render_layout)   //
        };
        if (subpass_descs[i].depth_attachment.attachment != UINT32_MAX) subpass_descs[i].has_depth = true;
    }

    Array<VkSubpassDescription> subpasses;
    subpasses.resize(subpass_descs.size());
    for (size_t i = 0; i < subpass_descs.size(); ++i)
    {
        auto& subpass_desc                   = subpass_descs[i];
        subpasses[i].pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpasses[i].colorAttachmentCount    = subpass_desc.color_attachments.size();
        subpasses[i].pColorAttachments       = subpass_desc.color_attachments.data();
        subpasses[i].pDepthStencilAttachment = subpass_desc.has_depth ? &subpass_desc.depth_attachment : nullptr;
    }

    VkRenderPassCreateInfo create_info = {};
    create_info.sType                  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    create_info.attachmentCount        = descriptions.size();
    create_info.pAttachments           = descriptions.data();
    create_info.subpassCount           = subpasses.size();
    create_info.pSubpasses             = subpasses.data();

    VERIFY_VULKAN_RESULT(vkCreateRenderPass(device, &create_info, nullptr, &handle_));
}