//
// Created by Echo on 2025/3/25.
//
#include "RenderPass.hpp"

#include "Enums.hpp"
#include "GfxContext.hpp"

using namespace NRHI;

struct MySubpassDesc {
    Array<VkAttachmentReference> color_attachments;
    VkAttachmentReference depth_attachment{.attachment = UINT32_MAX, .layout = VK_IMAGE_LAYOUT_UNDEFINED};
    bool has_depth = false;
};

RenderPass_Vulkan::RenderPass_Vulkan(const RenderPassCreateInfo &info) : RenderPass(info) {
    const auto &ctx = *GetVulkanGfxContext();
    const auto device = ctx.GetDevice();

    Array<VkAttachmentDescription> descriptions;
    descriptions.Resize(info.attachments.Count());
    for (size_t i = 0; i < info.attachments.Count(); ++i) {
        const auto &attachment = info.attachments[i];
        descriptions[i].format = RHIFormatToVkFormat(attachment.format);
        descriptions[i].samples = RHISampleCountToVkSampleCount(attachment.sample_count);
        descriptions[i].loadOp = RHIAttachmentLoadOpToVkAttachmentLoadOp(attachment.load_op);
        descriptions[i].storeOp = RHIAttachmentStoreOpToVkAttachmentStoreOp(attachment.store_op);
        descriptions[i].stencilLoadOp = RHIAttachmentLoadOpToVkAttachmentLoadOp(attachment.stencil_load_op);
        descriptions[i].stencilStoreOp = RHIAttachmentStoreOpToVkAttachmentStoreOp(attachment.stencil_store_op);
        descriptions[i].initialLayout = RHIImageLayoutToVkImageLayout(attachment.initial_layout);
        descriptions[i].finalLayout = RHIImageLayoutToVkImageLayout(attachment.final_layout);
    }

    Array<MySubpassDesc> subpass_descs;
    subpass_descs.Resize(info.subpasses.Count());
    for (size_t i = 0; i < info.subpasses.Count(); ++i) {
        const auto &subpass = info.subpasses[i];
        subpass_descs[i].color_attachments.Resize(subpass.color_attachments.Count());
        for (size_t j = 0; j < subpass.color_attachments.Count(); ++j) {
            const auto &attachment = subpass.color_attachments[j];
            subpass_descs[i].color_attachments[j].attachment = attachment.index;
            subpass_descs[i].color_attachments[j].layout = RHIImageLayoutToVkImageLayout(attachment.render_layout);
        }
        subpass_descs[i].depth_attachment = {
                subpass.depth_attachment.index, //
                RHIImageLayoutToVkImageLayout(subpass.depth_attachment.render_layout) //
        };
        if (subpass_descs[i].depth_attachment.attachment != UINT32_MAX)
            subpass_descs[i].has_depth = true;
    }

    Array<VkSubpassDescription> subpasses;
    subpasses.Resize(subpass_descs.Count());
    for (size_t i = 0; i < subpass_descs.Count(); ++i) {
        auto &subpass_desc = subpass_descs[i];
        subpasses[i].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpasses[i].colorAttachmentCount = subpass_desc.color_attachments.Count();
        subpasses[i].pColorAttachments = subpass_desc.color_attachments.Data();
        subpasses[i].pDepthStencilAttachment = subpass_desc.has_depth ? &subpass_desc.depth_attachment : nullptr;
    }

    VkRenderPassCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    create_info.attachmentCount = descriptions.Count();
    create_info.pAttachments = descriptions.Data();
    create_info.subpassCount = subpasses.Count();
    create_info.pSubpasses = subpasses.Data();

    VerifyVulkanResult(vkCreateRenderPass(device, &create_info, nullptr, &handle_));
}
