/**
 * @file GraphicsPipeline.cpp
 * @author Echo
 * @Date 24-4-27
 * @brief
 */

#include "GraphicsPipeline.h"

#include "Component/Camera.h"
#include "CoreGlobal.h"
#include "LogicalDevice.h"
#include "RenderPass.h"
#include "RHI/Vulkan/Resource/VulkanModel.h"
#include "RHI/Vulkan/VulkanContext.h"
#include "ShaderProgram.h"
#include "Utils/StringUtils.h"

#include <ranges>

namespace rhi::vulkan
{
GraphicsPipeline::~GraphicsPipeline()
{
    DestroyPipeline();
}

GraphicsPipeline::GraphicsPipeline(const PipelineInfo& pipeline_info)
{
    pipeline_info_ = pipeline_info;
    CreatePipeline();
}

void GraphicsPipeline::BindPipeline(vk::CommandBuffer cb) const
{
    cb.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline_);
}

void GraphicsPipeline::UpdateViewport(vk::CommandBuffer cb, const float width, const float height, const float x, const float y) const
{
    vk::Viewport viewport;
    viewport.x        = x;
    viewport.y        = y;
    viewport.width    = width == 0 ? g_engine_statistics.window_size.width : width;
    viewport.height   = height == 0 ? g_engine_statistics.window_size.height : height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    cb.setViewport(0, viewport);
}

void GraphicsPipeline::UpdateScissor(vk::CommandBuffer cb, const uint32_t width, const uint32_t height, const int offset_x, const int offset_y) const
{
    vk::Rect2D scisor;
    scisor.offset = vk::Offset2D{offset_x, offset_y};
    if (width == 0 || height == 0)
    {
        scisor.extent = VulkanContext::Get()->GetSwapChainExtent();
    }
    else
    {
        scisor.extent = vk::Extent2D{width, height};
    }
    cb.setScissor(0, scisor);
}

void GraphicsPipeline::BindVertexBuffers(vk::CommandBuffer cb, const Array<vk::Buffer>& buffers, const Array<vk::DeviceSize>& offsets) const
{
    cb.bindVertexBuffers(0, buffers, offsets);
}

void GraphicsPipeline::BindIndexBuffer(vk::CommandBuffer cb, const vk::Buffer buffer, const vk::DeviceSize offset) const
{
    cb.bindIndexBuffer(buffer, offset, vk::IndexType::eUint32);
}

void GraphicsPipeline::BindMesh(vk::CommandBuffer cb, const Mesh& mesh) const
{
    BindVertexBuffers(cb, {mesh.GetVertexBuffer()}, {0});
    BindIndexBuffer(cb, mesh.GetIndexBuffer());
}

void GraphicsPipeline::BindDescriptorSets(
    vk::CommandBuffer cb, const Array<vk::DescriptorSet>& descriptor_sets, const vk::PipelineBindPoint bind_point, const uint32_t first_set,
    const Array<uint32_t>& dynamic_offsets
) const
{
    cb.bindDescriptorSets(bind_point, pipeline_layout_, first_set, descriptor_sets, dynamic_offsets);
}

void GraphicsPipeline::DrawIndexed(
    vk::CommandBuffer cb, const uint32_t index_count, const uint32_t instance_count, const uint32_t first_index, const int32_t vertex_offset,
    const uint32_t first_instance
) const
{
    cb.drawIndexed(index_count, instance_count, first_index, vertex_offset, first_instance);
    g_engine_statistics.IncreaseDrawCall();
}

void GraphicsPipeline::Draw(vk::CommandBuffer cb, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance)
    const
{
    cb.draw(vertex_count, instance_count, first_vertex, first_instance);
    g_engine_statistics.IncreaseDrawCall();
}

Array<vk::DescriptorSet> GraphicsPipeline::GetCurrentFrameDescriptorSet() const
{
    return {shader_program_->GetDescriptorSets()[g_engine_statistics.current_frame_index]};
}

void GraphicsPipeline::CreatePipeline()
{
    VulkanContext& context = *VulkanContext::Get();
    /************************* 配置RenderPass ************************/
    if (pipeline_info_.render_pass == nullptr)
    {
        // TODO: 加载引擎默认RenderPass
        LOG_CRITIAL_CATEGORY(Vulkan.RenderPass, L"RenderPass不能为空");
    }
    render_pass_ = pipeline_info_.render_pass;
    render_pass_->Initialize();
    /************************* 配置RenderPass结束 ************************/

    /************************* 配置Shader ************************/
    if (pipeline_info_.shader_program == nullptr)
    {
        LOG_CRITIAL_CATEGORY(Vulkan, L"创建管线: {} 失败, 传入ShaderProgram为空", StringUtils::FromAnsiString(pipeline_info_.name_));
    }

    // TODO: Shader管理器
    shader_program_ = pipeline_info_.shader_program;

    // 配置Shader的阶段
    vk::PipelineShaderStageCreateInfo vert_info{};
    vert_info.setStage(vk::ShaderStageFlagBits::eVertex).setModule(shader_program_->GetVertShaderHandle()).setPName("main");

    // 配置frag shader的最大光照信息
    StaticArray<vk::SpecializationMapEntry, 1> specializations;
    specializations[0].constantID = 0;
    specializations[0].size       = sizeof(int32_t);
    specializations[0].offset     = 0;

    vk::SpecializationInfo specialization_info;
    specialization_info.dataSize = sizeof(int32_t);
    specialization_info.setMapEntries(specializations);
    specialization_info.pData = &g_engine_statistics.graphics.max_point_light_count;

    vk::PipelineShaderStageCreateInfo frag_info{};
    frag_info.setStage(vk::ShaderStageFlagBits::eFragment).setModule(shader_program_->GetFragShaderHandle()).setPName("main");
    frag_info.setPSpecializationInfo(&specialization_info);
    StaticArray<vk::PipelineShaderStageCreateInfo, 2> shader_stages = {vert_info, frag_info};

    // 配置顶点Shader的输入信息
    auto binding_desc   = shader_program_->GetVertexInputBindingDescription();
    auto attribute_desc = shader_program_->GetVertexInputAttributeDescriptions();

    vk::PipelineVertexInputStateCreateInfo vertex_input_info = {};
    vertex_input_info.setVertexAttributeDescriptions(attribute_desc);
    if (pipeline_info_.has_vertex_binding)
    {
        vertex_input_info.setVertexBindingDescriptions(binding_desc);
    }

    // 配置输入装配
    vk::PipelineInputAssemblyStateCreateInfo input_assembly_info = {};
    input_assembly_info                                      //
        .setTopology(vk::PrimitiveTopology::eTriangleList)   // 每三个顶点构成一个图元
        .setPrimitiveRestartEnable(false);
    /************************* Shader配置结束 ************************/

    /************************* 配置视口 ************************/
    vk::Viewport viewport_info     = {};
    pipeline_info_.viewport.width  = pipeline_info_.viewport.width == 0 ? context.GetSwapChainExtent().width : pipeline_info_.viewport.width;
    pipeline_info_.viewport.height = pipeline_info_.viewport.height == 0 ? context.GetSwapChainExtent().height : pipeline_info_.viewport.height;
    viewport_info.x                = pipeline_info_.viewport.x;
    viewport_info.y                = pipeline_info_.viewport.y;
    viewport_info.width            = pipeline_info_.viewport.width;
    viewport_info.height           = pipeline_info_.viewport.height;
    viewport_info.minDepth         = 0.f;
    viewport_info.maxDepth         = 1.f;

    pipeline_info_.clipping_rect.width =
        pipeline_info_.clipping_rect.width == 0 ? context.GetSwapChainExtent().width : pipeline_info_.clipping_rect.width;
    pipeline_info_.clipping_rect.height =
        pipeline_info_.clipping_rect.height == 0 ? context.GetSwapChainExtent().height : pipeline_info_.clipping_rect.height;
    vk::Rect2D scissor    = {};
    scissor.offset.x      = pipeline_info_.clipping_rect.offset_x;
    scissor.offset.y      = pipeline_info_.clipping_rect.offset_y;
    scissor.extent.width  = pipeline_info_.clipping_rect.width;
    scissor.extent.height = pipeline_info_.clipping_rect.height;

    vk::PipelineViewportStateCreateInfo viewport_state_create_info = {};
    viewport_state_create_info.setViewports({viewport_info}).setScissors({scissor});
    /************************* 配置视口结束 ************************/

    /************************* 配置光栅化 ************************/
    // 光栅化设置 将顶点着色器构成的几何图形转化为片段交给片段着色器着色
    vk::PipelineRasterizationStateCreateInfo rasterizer_info = {};
    // clang-format off
    rasterizer_info
        .setDepthClampEnable(pipeline_info_.rasterization_stage.enable_depth_clamp)
        .setRasterizerDiscardEnable(pipeline_info_.rasterization_stage.enable_raterizer_discard)
        .setPolygonMode(pipeline_info_.rasterization_stage.polygon_mode)
        .setLineWidth(pipeline_info_.rasterization_stage.line_width)
        .setCullMode(pipeline_info_.rasterization_stage.cull_mode)
        .setFrontFace(pipeline_info_.rasterization_stage.front_face)
        .setDepthBiasEnable(pipeline_info_.rasterization_stage.enable_depth_bias);
    // clang-format on
    /************************* 配置光栅化结束 ************************/

    /************************* 配置多重采样 ************************/
    vk::PipelineMultisampleStateCreateInfo multisample_info = {};

    sample_count_ = pipeline_info_.multisample.sample_count;
    multisample_info   //
        .setSampleShadingEnable(pipeline_info_.multisample.enabled)
        .setRasterizationSamples(pipeline_info_.multisample.sample_count);
    /************************* 配置多重采样结束 ************************/

    /************************* 配置深度模版测试 ************************/
    vk::PipelineDepthStencilStateCreateInfo depth_stencil_info = {};
    depth_stencil_info   //
        .setDepthTestEnable(pipeline_info_.depth_stencil_stage.enable_depth_test)
        .setDepthWriteEnable(pipeline_info_.depth_stencil_stage.enable_depth_write)
        .setDepthCompareOp(pipeline_info_.depth_stencil_stage.depth_compare_op)
        .setDepthBoundsTestEnable(pipeline_info_.depth_stencil_stage.enable_depth_bounds_test)
        .setStencilTestEnable(pipeline_info_.depth_stencil_stage.enable_stencil_test);
    /************************* 配置深度模版测试结束 ************************/

    /************************* 配置颜色混合 ************************/
    vk::PipelineColorBlendAttachmentState color_blend_attachment = {};
    color_blend_attachment   //
        .setColorWriteMask(
            vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
        )
        .setBlendEnable(pipeline_info_.color_blend_attachment_state.enabled);

    vk::PipelineColorBlendStateCreateInfo color_blend_info = {};
    color_blend_info   //
        .setLogicOpEnable(false)
        .setLogicOp(vk::LogicOp::eCopy)   // 逻辑操作
        .setAttachments({color_blend_attachment});
    /************************* 配置颜色混合结束 ************************/

    // 指定管线布局(uniform)
    vk::PipelineLayoutCreateInfo pipeline_layout_info = {};
    StaticArray                 layout               = {shader_program_->GetDescriptorSetLayout()};
    pipeline_layout_info.setSetLayouts(layout);

    Array<vk::PushConstantRange> push_constant_ranges;

    // PushConstants
    for (const auto& push_constant: shader_program_->GetVertexPushConstants())
    {
        vk::PushConstantRange range;
        range.stageFlags = ShaderStage2VKShaderStage(push_constant.stage);
        range.offset     = push_constant.offset;
        range.size       = push_constant.size;
        push_constant_ranges.emplace_back(range);
    }
    for (const auto& push_constant: shader_program_->GetFragmentPushConstants())
    {
        vk::PushConstantRange range;
        range.stageFlags = ShaderStage2VKShaderStage(push_constant.stage);
        range.offset     = push_constant.offset;
        range.size       = push_constant.size;
        push_constant_ranges.emplace_back(range);
    }

    pipeline_layout_info.setPushConstantRanges(push_constant_ranges);

    if (!pipeline_info_.name_.empty())
    {
        pipeline_info_.pipeline_layout_name_ = pipeline_info_.name_ + "_PipelineLayout";
    }

    // 创建管线布局
    pipeline_layout_ = context.GetLogicalDevice()->CreatePipelineLayout(pipeline_layout_info, pipeline_info_.pipeline_layout_name_);

    // DynamicState
    vk::PipelineDynamicStateCreateInfo dynamic_state_info;
    Array<vk::DynamicState>           dynamic_states;
    if (!(pipeline_info_.dynamic_state_enabled & EPDSE_None))
    {
        if (pipeline_info_.dynamic_state_enabled & EPDSE_Scissor)
        {
            dynamic_states.emplace_back(vk::DynamicState::eScissor);
        }
        if (pipeline_info_.dynamic_state_enabled & EPDSE_Viewport)
        {
            dynamic_states.emplace_back(vk::DynamicState::eViewport);
        }
    }
    dynamic_state_info.setDynamicStates(dynamic_states);

    // 定义管线
    vk::GraphicsPipelineCreateInfo pipeline_create_info = {};
    pipeline_create_info.setStages(shader_stages)
        .setPVertexInputState(&vertex_input_info)
        .setPInputAssemblyState(&input_assembly_info)
        .setPViewportState(&viewport_state_create_info)
        .setPRasterizationState(&rasterizer_info)
        .setPMultisampleState(&multisample_info)
        .setPDepthStencilState(&depth_stencil_info)   // 深度和模版测试
        .setPColorBlendState(&color_blend_info)       // 颜色混合
        .setLayout(pipeline_layout_)                  // 管线布局
        .setRenderPass(render_pass_->GetHandle())     // RenderPass
        .setSubpass(0)                                // 子Pass
        .setPDynamicState(&dynamic_state_info);

    if (!pipeline_info_.name_.empty())
    {
        pipeline_info_.pipeline_name_ = pipeline_info_.name_ + "_Pipeline";
    }

    pipeline_ = context.GetLogicalDevice()->CreateGraphicsPipeline(nullptr, pipeline_create_info, pipeline_info_.pipeline_name_);
}

void GraphicsPipeline::DestroyPipeline()
{
    const auto& device = VulkanContext::Get()->GetLogicalDevice();
    if (device)
    {
        device->GetHandle().destroyPipeline(pipeline_);
        device->GetHandle().destroyPipelineLayout(pipeline_layout_);
        render_pass_     = nullptr;
        pipeline_        = nullptr;
        pipeline_layout_ = nullptr;
    }
}

void GraphicsPipeline::Rebuild()
{
    if (!(pipeline_info_.dynamic_state_enabled | EPDSE_None)) {
        // 启用了DynamicState则通过DynamicState设置
        DestroyPipeline();
        CreatePipeline();
    }
}
}
