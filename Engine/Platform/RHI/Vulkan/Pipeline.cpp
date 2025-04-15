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

GraphicsPipeline_Vulkan::GraphicsPipeline_Vulkan(const GraphicsPipelineDesc &Desc, const RenderPass *render_pass)
{
    auto decive = GetVulkanGfxContext()->GetDevice();
    Array<VkPipelineShaderStageCreateInfo> shader_stages;
    shader_stages.Resize(Desc.shaders.Count());
    for (size_t i = 0; i < Desc.shaders.Count(); ++i)
    {
        shader_stages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[i].stage = static_cast<VkShaderStageFlagBits>(
            RHIShaderStageToVkShaderStage(Desc.shaders[i].stage));
        shader_stages[i].module = Desc.shaders[i].shader->GetNativeHandleT<VkShaderModule>();
        shader_stages[i].pName = "main";
    }
    Array<VkVertexInputBindingDescription> input_bindings;
    input_bindings.Resize(Desc.vertex_inputs.Count());
    for (size_t i = 0; i < Desc.vertex_inputs.Count(); ++i)
    {
        const auto &input = Desc.vertex_inputs[i];
        input_bindings[i].binding = input.Binding;
        input_bindings[i].stride = input.Stride;
        input_bindings[i].inputRate = RHIVertexInputRateToVkVertexInputRate(input.Rate);
    }

    Array<VkVertexInputAttributeDescription> input_attributes;
    input_attributes.Resize(Desc.vertex_attributes.Count());
    for (size_t i = 0; i < Desc.vertex_attributes.Count(); ++i)
    {
        const auto &attribute = Desc.vertex_attributes[i];
        input_attributes[i].location = attribute.Location;
        input_attributes[i].binding = attribute.Binding;
        input_attributes[i].format = RHIFormatToVkFormat(attribute.Format);
        input_attributes[i].offset = attribute.Offset;
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
    viewport.x = Desc.viewport.x;
    viewport.y = Desc.viewport.y;
    viewport.width = Desc.viewport.w;
    viewport.height = Desc.viewport.h;

    VkRect2D scissor{};
    scissor.offset.x = Desc.scissor.x;
    scissor.offset.y = Desc.scissor.y;
    scissor.extent.width = Desc.scissor.w;
    scissor.extent.height = Desc.scissor.h;

    VkPipelineViewportStateCreateInfo viewport_info{};
    viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_info.viewportCount = 1;
    viewport_info.pViewports = &viewport;
    viewport_info.scissorCount = 1;
    viewport_info.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterization_info{};
    rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_info.depthClampEnable = Desc.rasterization.enable_depth_clamp;
    rasterization_info.rasterizerDiscardEnable = Desc.rasterization.enable_discard_rasterizer;
    rasterization_info.polygonMode = RHIPolygonModeToVkPolygonMode(Desc.rasterization.polygon_mode);
    rasterization_info.lineWidth = Desc.rasterization.line_width;
    rasterization_info.cullMode = RHICullModeToVkCullMode(Desc.rasterization.cull_mode);
    rasterization_info.frontFace = RHIFrontFaceToVkFrontFace(Desc.rasterization.front_face);
    rasterization_info.depthBiasEnable = Desc.rasterization.enable_depth_bias;

    VkPipelineMultisampleStateCreateInfo multisample_info{};
    multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_info.rasterizationSamples = RHISampleCountToVkSampleCount(Desc.multisample.count);
    multisample_info.sampleShadingEnable = Desc.multisample.enable_sample_shading;

    VkPipelineDepthStencilStateCreateInfo depth_stencil_info{};
    depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil_info.depthTestEnable = Desc.depth_stencil.enable_depth_test;
    depth_stencil_info.depthWriteEnable = Desc.depth_stencil.enable_depth_write;
    depth_stencil_info.depthCompareOp = RHICompareOpToVkCompareOp(Desc.depth_stencil.depth_compare_op);
    depth_stencil_info.stencilTestEnable = Desc.depth_stencil.enable_stencil_test;

    VkPipelineColorBlendAttachmentState color_blend_attachment{};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_FALSE;
    if (Desc.color_blend.enable_blend)
    {
        color_blend_attachment.blendEnable = VK_TRUE;
        color_blend_attachment.srcColorBlendFactor = RHIBlendFactorToVkBlendFactor(
            Desc.color_blend.src_color_blend_factor);
        color_blend_attachment.dstColorBlendFactor = RHIBlendFactorToVkBlendFactor(
            Desc.color_blend.dst_color_blend_factor);
        color_blend_attachment.colorBlendOp = RHIBlendOpToVkBlendOp(Desc.color_blend.color_blend_op);
        color_blend_attachment.srcAlphaBlendFactor = RHIBlendFactorToVkBlendFactor(
            Desc.color_blend.src_alpha_blend_factor);
        color_blend_attachment.dstAlphaBlendFactor = RHIBlendFactorToVkBlendFactor(
            Desc.color_blend.dst_alpha_blend_factor);
        color_blend_attachment.alphaBlendOp = RHIBlendOpToVkBlendOp(Desc.color_blend.alpha_blend_op);
    }

    VkPipelineColorBlendStateCreateInfo color_blend_info{};
    color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_info.attachmentCount = 1;
    color_blend_info.pAttachments = &color_blend_attachment;
    color_blend_info.logicOpEnable = VK_FALSE;

    VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
    PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    PipelineLayoutInfo.setLayoutCount = Desc.descriptor_set_layouts.Count();

    Array<VkDescriptorSetLayout> layouts_native =
        Desc.descriptor_set_layouts |
        range::view::Select([](const SharedPtr<DescriptorSetLayout> &layout) {
            return layout->GetNativeHandleT<VkDescriptorSetLayout>();
        }) |
        range::To<Array<VkDescriptorSetLayout> >();
    descriptor_layouts_ = Desc.descriptor_set_layouts;
    PipelineLayoutInfo.pSetLayouts = layouts_native.Data();
    Array<VkPushConstantRange> PushConstants;
    for (const auto &Layout : Desc.PushConstants)
    {
        VkPushConstantRange Range;
        Range.offset = Layout.Offset;
        Range.size = Layout.Size;
        Range.stageFlags = RHIShaderStageToVkShaderStage(Layout.Stage);
        PushConstants.Add(Range);
    }
    PipelineLayoutInfo.pushConstantRangeCount = PushConstants.Count();
    PipelineLayoutInfo.pPushConstantRanges = PushConstants.Data();

    VerifyVulkanResult(vkCreatePipelineLayout(decive, &PipelineLayoutInfo, nullptr, &pipeline_layout_));

    VkDynamicState dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamic_state_info{};
    dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_info.dynamicStateCount = SizeOf(dynamic_states);
    dynamic_state_info.pDynamicStates = dynamic_states;

    VkGraphicsPipelineCreateInfo PipelineInfo{};
    PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    PipelineInfo.stageCount = shader_stages.Count();
    PipelineInfo.pStages = shader_stages.Data();
    PipelineInfo.pVertexInputState = &vertex_input_info;
    PipelineInfo.pInputAssemblyState = &input_assembly_info;
    PipelineInfo.pViewportState = &viewport_info;
    PipelineInfo.pRasterizationState = &rasterization_info;
    PipelineInfo.pMultisampleState = &multisample_info;
    PipelineInfo.pDepthStencilState = &depth_stencil_info;
    PipelineInfo.pColorBlendState = &color_blend_info;
    PipelineInfo.pDynamicState = &dynamic_state_info;
    PipelineInfo.layout = pipeline_layout_;
    // TODO: PSO Cache
    PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    if (render_pass == nullptr)
    {
        dynamic_rendering_ = true;
        VkPipelineRenderingCreateInfo rendering_info{};

        rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        rendering_info.depthAttachmentFormat = RHIFormatToVkFormat(Desc.attachments.depth_format);
        rendering_info.stencilAttachmentFormat = RHIFormatToVkFormat(Desc.attachments.stencil_format);
        Array<VkFormat> color_formats = Desc.attachments.color_formats |
                                        range::view::Select([](Format format) {
                                            return RHIFormatToVkFormat(format);
                                        }) | range::To<Array<VkFormat> >();
        rendering_info.colorAttachmentCount = color_formats.Count();
        rendering_info.pColorAttachmentFormats = color_formats.Data();

        PipelineInfo.pNext = &rendering_info;
        PipelineInfo.renderPass = VK_NULL_HANDLE;
        VerifyVulkanResult(vkCreateGraphicsPipelines(decive, VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &pipeline_));
    }
    else
    {
        dynamic_rendering_ = false;
        PipelineInfo.renderPass = render_pass->GetNativeHandleT<VkRenderPass>();
        PipelineInfo.subpass = 0;
        VerifyVulkanResult(vkCreateGraphicsPipelines(decive, VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &pipeline_));
    }
}

GraphicsPipeline_Vulkan::~GraphicsPipeline_Vulkan()
{
    auto &ctx = *GetVulkanGfxContext();
    auto device = ctx.GetDevice();
    vkDestroyPipeline(device, pipeline_, nullptr);
    vkDestroyPipelineLayout(device, pipeline_layout_, nullptr);
}

ComputePipeline_Vulkan::ComputePipeline_Vulkan(const ComputePipelineDesc &desc)
{
    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 1;
    VkDescriptorSetLayout set_layout = desc.pipline_layout->GetNativeHandleT<VkDescriptorSetLayout>();
    pipeline_layout_info.pSetLayouts = &set_layout;
    VerifyVulkanResult(vkCreatePipelineLayout(GetVulkanGfxContext()->GetDevice(), &pipeline_layout_info, nullptr,
                                              &pipeline_layout_));
    VkComputePipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipeline_info.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipeline_info.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    pipeline_info.stage.module = desc.shader.shader->GetNativeHandleT<VkShaderModule>();
    pipeline_info.stage.pName = "main";
    pipeline_info.layout = pipeline_layout_;
    VerifyVulkanResult(vkCreateComputePipelines(GetVulkanGfxContext()->GetDevice(), VK_NULL_HANDLE, 1, &pipeline_info,
                                                nullptr, &pipeline_));
}

ComputePipeline_Vulkan::~ComputePipeline_Vulkan()
{
    vkDestroyPipeline(GetVulkanGfxContext()->GetDevice(), pipeline_, nullptr);
    vkDestroyPipelineLayout(GetVulkanGfxContext()->GetDevice(), pipeline_layout_, nullptr);
}