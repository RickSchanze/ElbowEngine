//
// Created by Echo on 2025/3/25.
//
#include "Pipeline.hpp"

#include "Enums.hpp"
#include "GfxContext.hpp"
#include "Platform/RHI/DescriptorSet.hpp"
#include "Platform/RHI/LowShader.hpp"
#include "Platform/RHI/RenderPass.hpp"

using namespace RHI;

GraphicsPipeline_Vulkan::GraphicsPipeline_Vulkan(const GraphicsPipelineDesc &desc, const RenderPass *render_pass) {
    auto decive = GetVulkanGfxContext()->GetDevice();
    Array<VkPipelineShaderStageCreateInfo> shader_stages;
    shader_stages.Resize(desc.shaders.Count());
    for (size_t i = 0; i < desc.shaders.Count(); ++i) {
        shader_stages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[i].stage = static_cast<VkShaderStageFlagBits>(RHIShaderStageToVkShaderStage(desc.shaders[i].stage));
        shader_stages[i].module = desc.shaders[i].shader->GetNativeHandleT<VkShaderModule>();
        shader_stages[i].pName = "main";
    }
    Array<VkVertexInputBindingDescription> input_bindings;
    input_bindings.Resize(desc.vertex_inputs.Count());
    for (size_t i = 0; i < desc.vertex_inputs.Count(); ++i) {
        const auto &input = desc.vertex_inputs[i];
        input_bindings[i].binding = input.binding;
        input_bindings[i].stride = input.stride;
        input_bindings[i].inputRate = RHIVertexInputRateToVkVertexInputRate(input.rate);
    }

    Array<VkVertexInputAttributeDescription> input_attributes;
    input_attributes.Resize(desc.vertex_attributes.Count());
    for (size_t i = 0; i < desc.vertex_attributes.Count(); ++i) {
        const auto &attribute = desc.vertex_attributes[i];
        input_attributes[i].location = attribute.location;
        input_attributes[i].binding = attribute.binding;
        input_attributes[i].format = RHIFormatToVkFormat(attribute.format);
        input_attributes[i].offset = attribute.offset;
    }

    VkPipelineVertexInputStateCreateInfo vertex_input_info{};
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = input_bindings.Count();
    vertex_input_info.pVertexBindingDescriptions = input_bindings.Data();
    vertex_input_info.vertexAttributeDescriptionCount = input_attributes.Count();
    vertex_input_info.pVertexAttributeDescriptions = input_attributes.Data();

    VkPipelineInputAssemblyStateCreateInfo input_assembly_info{};
    input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_info.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = desc.viewport.x;
    viewport.y = desc.viewport.y;
    viewport.width = desc.viewport.w;
    viewport.height = desc.viewport.h;

    VkRect2D scissor{};
    scissor.offset.x = desc.scissor.x;
    scissor.offset.y = desc.scissor.y;
    scissor.extent.width = desc.scissor.w;
    scissor.extent.height = desc.scissor.h;

    VkPipelineViewportStateCreateInfo viewport_info{};
    viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_info.viewportCount = 1;
    viewport_info.pViewports = &viewport;
    viewport_info.scissorCount = 1;
    viewport_info.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterization_info{};
    rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_info.depthClampEnable = desc.rasterization.enable_depth_clamp;
    rasterization_info.rasterizerDiscardEnable = desc.rasterization.enable_discard_rasterizer;
    rasterization_info.polygonMode = RHIPolygonModeToVkPolygonMode(desc.rasterization.polygon_mode);
    rasterization_info.lineWidth = desc.rasterization.line_width;
    rasterization_info.cullMode = RHICullModeToVkCullMode(desc.rasterization.cull_mode);
    rasterization_info.frontFace = RHIFrontFaceToVkFrontFace(desc.rasterization.front_face);
    rasterization_info.depthBiasEnable = desc.rasterization.enable_depth_bias;

    VkPipelineMultisampleStateCreateInfo multisample_info{};
    multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_info.rasterizationSamples = RHISampleCountToVkSampleCount(desc.multisample.count);
    multisample_info.sampleShadingEnable = desc.multisample.enable_sample_shading;

    VkPipelineDepthStencilStateCreateInfo depth_stencil_info{};
    depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil_info.depthTestEnable = desc.depth_stencil.enable_depth_test;
    depth_stencil_info.depthWriteEnable = desc.depth_stencil.enable_depth_write;
    depth_stencil_info.depthCompareOp = RHICompareOpToVkCompareOp(desc.depth_stencil.depth_compare_op);
    depth_stencil_info.stencilTestEnable = desc.depth_stencil.enable_stencil_test;

    VkPipelineColorBlendAttachmentState color_blend_attachment{};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_FALSE;
    if (desc.color_blend.enable_blend) {
        color_blend_attachment.blendEnable = VK_TRUE;
        color_blend_attachment.srcColorBlendFactor = RHIBlendFactorToVkBlendFactor(desc.color_blend.src_color_blend_factor);
        color_blend_attachment.dstColorBlendFactor = RHIBlendFactorToVkBlendFactor(desc.color_blend.dst_color_blend_factor);
        color_blend_attachment.colorBlendOp = RHIBlendOpToVkBlendOp(desc.color_blend.color_blend_op);
        color_blend_attachment.srcAlphaBlendFactor = RHIBlendFactorToVkBlendFactor(desc.color_blend.src_alpha_blend_factor);
        color_blend_attachment.dstAlphaBlendFactor = RHIBlendFactorToVkBlendFactor(desc.color_blend.dst_alpha_blend_factor);
        color_blend_attachment.alphaBlendOp = RHIBlendOpToVkBlendOp(desc.color_blend.alpha_blend_op);
    }

    VkPipelineColorBlendStateCreateInfo color_blend_info{};
    color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_info.attachmentCount = 1;
    color_blend_info.pAttachments = &color_blend_attachment;
    color_blend_info.logicOpEnable = VK_FALSE;

    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = desc.descriptor_set_layouts.Count();

    Array<VkDescriptorSetLayout> layouts_native =
            desc.descriptor_set_layouts |
            range::view::Select([](const SharedPtr<DescriptorSetLayout> &layout) { return layout->GetNativeHandleT<VkDescriptorSetLayout>(); }) |
            range::To<Array<VkDescriptorSetLayout>>();
    descriptor_layouts_ = desc.descriptor_set_layouts;
    pipeline_layout_info.pSetLayouts = layouts_native.Data();
    VerifyVulkanResult(vkCreatePipelineLayout(decive, &pipeline_layout_info, nullptr, &pipeline_layout_));

    VkDynamicState dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamic_state_info{};
    dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_info.dynamicStateCount = SizeOf(dynamic_states);
    dynamic_state_info.pDynamicStates = dynamic_states;

    VkGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = shader_stages.Count();
    pipeline_info.pStages = shader_stages.Data();
    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &input_assembly_info;
    pipeline_info.pViewportState = &viewport_info;
    pipeline_info.pRasterizationState = &rasterization_info;
    pipeline_info.pMultisampleState = &multisample_info;
    pipeline_info.pDepthStencilState = &depth_stencil_info;
    pipeline_info.pColorBlendState = &color_blend_info;
    pipeline_info.pDynamicState = &dynamic_state_info;
    pipeline_info.layout = pipeline_layout_;
    // TODO: PSO Cache
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    if (render_pass == nullptr) {
        dynamic_rendering_ = true;
        VkPipelineRenderingCreateInfo rendering_info{};

        rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        rendering_info.depthAttachmentFormat = RHIFormatToVkFormat(desc.attachments.depth_format);
        rendering_info.stencilAttachmentFormat = RHIFormatToVkFormat(desc.attachments.stencil_format);
        Array<VkFormat> color_formats = desc.attachments.color_formats |
                                        range::view::Select([](Format format) { return RHIFormatToVkFormat(format); }) | range::To<Array<VkFormat>>();
        rendering_info.colorAttachmentCount = color_formats.Count();
        rendering_info.pColorAttachmentFormats = color_formats.Data();

        pipeline_info.pNext = &rendering_info;
        pipeline_info.renderPass = VK_NULL_HANDLE;
        VerifyVulkanResult(vkCreateGraphicsPipelines(decive, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline_));
    } else {
        dynamic_rendering_ = false;
        pipeline_info.renderPass = render_pass->GetNativeHandleT<VkRenderPass>();
        pipeline_info.subpass = 0;
        VerifyVulkanResult(vkCreateGraphicsPipelines(decive, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline_));
    }
}

GraphicsPipeline_Vulkan::~GraphicsPipeline_Vulkan() {
    auto &ctx = *GetVulkanGfxContext();
    auto device = ctx.GetDevice();
    vkDestroyPipeline(device, pipeline_, nullptr);
    vkDestroyPipelineLayout(device, pipeline_layout_, nullptr);
}

ComputePipeline_Vulkan::ComputePipeline_Vulkan(const ComputePipelineDesc &desc) {
    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 1;
    VkDescriptorSetLayout set_layout = desc.pipline_layout->GetNativeHandleT<VkDescriptorSetLayout>();
    pipeline_layout_info.pSetLayouts = &set_layout;
    VerifyVulkanResult(vkCreatePipelineLayout(GetVulkanGfxContext()->GetDevice(), &pipeline_layout_info, nullptr, &pipeline_layout_));
    VkComputePipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipeline_info.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipeline_info.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    pipeline_info.stage.module = desc.shader.shader->GetNativeHandleT<VkShaderModule>();
    pipeline_info.stage.pName = "main";
    pipeline_info.layout = pipeline_layout_;
    VerifyVulkanResult(vkCreateComputePipelines(GetVulkanGfxContext()->GetDevice(), VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline_));
}

ComputePipeline_Vulkan::~ComputePipeline_Vulkan() {
    vkDestroyPipeline(GetVulkanGfxContext()->GetDevice(), pipeline_, nullptr);
    vkDestroyPipelineLayout(GetVulkanGfxContext()->GetDevice(), pipeline_layout_, nullptr);
}
