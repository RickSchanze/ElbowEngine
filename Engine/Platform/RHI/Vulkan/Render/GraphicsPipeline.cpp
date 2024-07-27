/**
 * @file GraphicsPipeline.cpp
 * @author Echo
 * @Date 24-4-27
 * @brief
 */

#include "GraphicsPipeline.h"

#include "CommandPool.h"
#include "Component/Camera.h"
#include "CoreGlobal.h"
#include "LogicalDevice.h"
#include "RenderPass.h"
#include "RHI/Vulkan/Resource/VulkanModel.h"
#include "RHI/Vulkan/VulkanContext.h"
#include "ShaderProgram.h"
#include "Utils/StringUtils.h"

#include <ranges>

RHI_VULKAN_NAMESPACE_BEGIN

GraphicsPipeline::~GraphicsPipeline()
{
    DestroyCommandBuffers();
    DestroyPipeline();
    delete shader_program_;
}

GraphicsPipeline::GraphicsPipeline(const PipelineInfo& pipeline_info)
{
    pipeline_info_ = pipeline_info;
    CreatePipeline();
    CreateCommandBuffers();
}

vk::CommandBuffer GraphicsPipeline::GetCurrentCommandBuffer() const
{
    return command_buffers_[g_engine_statistics.current_image_index];
}

void GraphicsPipeline::BeginCommandBuffer()
{
    binded_buffer_ = command_buffers_[g_engine_statistics.current_image_index];
    vk::CommandBufferBeginInfo begin_info;
    begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    binded_buffer_.begin(begin_info);
}

void GraphicsPipeline::EndCommandBuffer()
{
    binded_buffer_.end();
    binded_buffer_ = nullptr;
}

void GraphicsPipeline::BeginRenderPass(const Color clear_color) const
{
    ASSERT_CATEGORY(Vulkan.Render, render_pass_ != nullptr, L"RenderPass不能为空");

    TStaticArray<vk::ClearValue, 2> clear_values;
    clear_values[0].color        = vk::ClearColorValue{TStaticArray<float, 4>{clear_color.r, clear_color.g, clear_color.b, clear_color.a}};
    clear_values[1].depthStencil = vk::ClearDepthStencilValue{1.0f, 0};

    vk::RenderPassBeginInfo render_pass_info;
    render_pass_info.renderPass        = render_pass_->GetHandle();
    render_pass_info.framebuffer       = render_pass_->GetCurrentFrameBufferHandle();
    render_pass_info.renderArea.offset = vk::Offset2D{0, 0};
    render_pass_info.renderArea.extent = VulkanContext::Get()->GetSwapChainExtent();
    render_pass_info.setClearValues(clear_values);
    binded_buffer_.beginRenderPass(render_pass_info, vk::SubpassContents::eInline);
}

void GraphicsPipeline::EndRenderPass() const
{
    binded_buffer_.endRenderPass();
}

void GraphicsPipeline::BindPipeline() const
{
    binded_buffer_.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline_);
}

void GraphicsPipeline::UpdateViewport(const float width, const float height, const float x, const float y) const
{
    vk::Viewport viewport;
    viewport.x        = x;
    viewport.y        = y;
    viewport.width    = width == 0 ? g_engine_statistics.window_size.width : width;
    viewport.height   = height == 0 ? g_engine_statistics.window_size.height : height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    binded_buffer_.setViewport(0, viewport);
}

void GraphicsPipeline::UpdateScissor(const uint32_t width, const uint32_t height, const int offset_x, const int offset_y) const
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
    binded_buffer_.setScissor(0, scisor);
}

void GraphicsPipeline::BindVertexBuffers(const TArray<vk::Buffer>& buffers, const TArray<vk::DeviceSize>& offsets) const
{
    binded_buffer_.bindVertexBuffers(0, buffers, offsets);
}

void GraphicsPipeline::BindIndexBuffer(const vk::Buffer buffer, const vk::DeviceSize offset) const
{
    binded_buffer_.bindIndexBuffer(buffer, offset, vk::IndexType::eUint32);
}

void GraphicsPipeline::BindMesh(const Mesh& mesh) const
{
    BindVertexBuffers({mesh.GetVertexBuffer()}, {0});
    BindIndexBuffer(mesh.GetIndexBuffer());
}

void GraphicsPipeline::BindDescriptiorSets(
    const TArray<vk::DescriptorSet>& descriptor_sets, const vk::PipelineBindPoint bind_point, const uint32_t first_set,
    const TArray<uint32_t>& dynamic_offsets
) const
{
    binded_buffer_.bindDescriptorSets(bind_point, pipeline_layout_, first_set, descriptor_sets, dynamic_offsets);
}

void GraphicsPipeline::DrawIndexed(
    const uint32_t index_count, const uint32_t instance_count, const uint32_t first_index, const int32_t vertex_offset, const uint32_t first_instance
) const
{
    binded_buffer_.drawIndexed(index_count, instance_count, first_index, vertex_offset, first_instance);
    g_engine_statistics.IncreaseDrawCall();
}

TArray<vk::DescriptorSet> GraphicsPipeline::GetCurrentFrameDescriptorSet() const
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
    vk::PipelineShaderStageCreateInfo frag_info{};
    frag_info.setStage(vk::ShaderStageFlagBits::eFragment).setModule(shader_program_->GetFragShaderHandle()).setPName("main");
    TStaticArray<vk::PipelineShaderStageCreateInfo, 2> shader_stages = {vert_info, frag_info};

    // 配置顶点Shader的输入信息
    auto binding_desc   = shader_program_->GetVertexInputBindingDescription();
    auto attribute_desc = shader_program_->GetVertexInputAttributeDescriptions();

    vk::PipelineVertexInputStateCreateInfo VertexInputInfo = {};
    VertexInputInfo   //
        .setVertexBindingDescriptions(binding_desc)
        .setVertexAttributeDescriptions(attribute_desc);

    // 配置输入装配
    vk::PipelineInputAssemblyStateCreateInfo InputAssemblyInfo = {};
    InputAssemblyInfo                                        //
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
    TStaticArray                 layout               = {shader_program_->GetDescriptorSetLayout()};
    pipeline_layout_info.setSetLayouts(layout);

    if (!pipeline_info_.name_.empty())
    {
        pipeline_info_.pipeline_layout_name_ = pipeline_info_.name_ + "_PipelineLayout";
    }

    // 创建管线布局
    pipeline_layout_ = context.GetLogicalDevice()->CreatePipelineLayout(pipeline_layout_info, pipeline_info_.pipeline_layout_name_);

    // DynamicState
    vk::PipelineDynamicStateCreateInfo dynamic_state_info;
    TArray<vk::DynamicState>           dynamic_states;
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
        .setPVertexInputState(&VertexInputInfo)
        .setPInputAssemblyState(&InputAssemblyInfo)
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
        delete render_pass_;
        render_pass_     = nullptr;
        pipeline_        = nullptr;
        pipeline_layout_ = nullptr;
    }
}

void GraphicsPipeline::CreateCommandBuffers()
{
    VulkanContext& context = *VulkanContext::Get();
    const auto&    pool    = context.GetCommandPool();

    vk::CommandBufferAllocateInfo alloc_info = {};
    alloc_info.level                         = vk::CommandBufferLevel::ePrimary;
    alloc_info.commandPool                   = pool->GetHandle();
    alloc_info.commandBufferCount            = g_engine_statistics.graphics.swapchain_image_count;

    AnsiString command_buffer_name = pipeline_info_.name_ + "_Command_Buffer";
    command_buffers_ = pool->CreateCommandBuffers(alloc_info, command_buffer_name.c_str(), &pipeline_info_.command_buffer_names);
}

void GraphicsPipeline::DestroyCommandBuffers()
{
    VulkanContext& context = *VulkanContext::Get();
    context.GetCommandPool()->DestroyCommandBuffers(command_buffers_);
    command_buffers_.clear();
    pipeline_info_.command_buffer_names.clear();
}

void GraphicsPipeline::Rebuild()
{
    if (!(pipeline_info_.dynamic_state_enabled | EPDSE_None)) {
        // 启用了DynamicState则通过DynamicState设置
        DestroyPipeline();
        CreatePipeline();
    }
}

RHI_VULKAN_NAMESPACE_END
