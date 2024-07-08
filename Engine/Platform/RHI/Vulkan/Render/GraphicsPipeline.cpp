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

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>
#include <ranges>

RHI_VULKAN_NAMESPACE_BEGIN

GraphicsPipeline::~GraphicsPipeline()
{
    CleanOther(false);
    CleanPipeline();
    VulkanContext::Get().RemovePipelineFromRender(this);
}

GraphicsPipeline::GraphicsPipeline(const PipelineInfo& pipeline_info)
{
    VulkanContext& Context = VulkanContext::Get();
    pipeline_info_         = pipeline_info;
    CreatePipeline();
    CreateOther(false);
    Context.AddPipelineToRender(this);
    LOG_INFO_CATEGORY(Vulkan, L"图形管线初始化完成");
}

void GraphicsPipeline::UpdateUniformBuffer(const UInt32 InCurrentImage) const
{
    VulkanContext& context      = VulkanContext::Get();
    static auto    start_time   = std::chrono::high_resolution_clock::now();
    const auto     current_time = std::chrono::high_resolution_clock::now();
    const float    time         = std::chrono::duration<float>(current_time - start_time).count();
    TStaticArray<glm::mat4, 3> ubo;
    ubo[0] = glm::mat4(1.f);
    // TODO: Here
    ubo[0] = rotate(ubo[0], glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
    // 缩放
    ubo[0] = scale(ubo[0], glm::vec3(0.05f, 0.05f, 0.05f));
    // 绕X转90度
    ubo[0] = rotate(ubo[0], glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
    ubo[1] = Function::Camera::Main->GetViewMatrix();

    ubo[2] = glm::perspective(
        glm::radians(45.f),
        static_cast<float>(context.GetSwapChainExtent().width) /
            static_cast<float>(context.GetSwapChainExtent().height),
        0.1f,
        10.f
    );
    ubo[2][1][1] *= -1;
    shader_program_->SetUniformBufferObject(ubo[0], ubo[1], ubo[2]);
}

vk::CommandBuffer GraphicsPipeline::GetCurrentImageCommandBuffer(const UInt32 InCurrentImage) const
{
    return command_buffers_[InCurrentImage];
}

void GraphicsPipeline::CreatePipeline()
{
    VulkanContext& context = VulkanContext::Get();
    /************************* 配置RenderPass ************************/
    if (pipeline_info_.render_pass == nullptr)
    {
        // RenderPass或许可以共用
        // @TODO: 探索RenderPass共用
        render_pass_              = new RenderPass();
        render_pass_->SampleCount = pipeline_info_.multisample.sample_count;
    }
    render_pass_->Initialize();
    /************************* 配置RenderPass结束 ************************/

    /************************* 配置Shader ************************/
    if (pipeline_info_.shader_stage.frag == nullptr || pipeline_info_.shader_stage.vert == nullptr)
    {
        // TODO: 加载引擎默认shader
        LOG_CRITIAL_CATEGORY(Vulkan.Shader, L"顶点或片元着色器不能为空");
    }

    // TODO: Shader管理器
    m_shader_prog_ = ShaderProgram::CreateShared(
        pipeline_info_.shader_stage.vert, pipeline_info_.shader_stage.vert
    );

    // 配置Shader的阶段
    vk::PipelineShaderStageCreateInfo VertInfo{};
    VertInfo.setStage(vk::ShaderStageFlagBits::eVertex)
        .setModule(shader_program_->GetVertShaderHandle())
        .setPName("main");
    vk::PipelineShaderStageCreateInfo FragInfo{};
    FragInfo.setStage(vk::ShaderStageFlagBits::eFragment)
        .setModule(shader_program_->GetFragShaderHandle())
        .setPName("main");
    TStaticArray<vk::PipelineShaderStageCreateInfo, 2> ShaderStages = {VertInfo, FragInfo};

    // 配置顶点Shader的输入信息
    auto BindingDesc   = m_shader_prog_->GetVertexInputBindingDescription();
    auto AttributeDesc = m_shader_prog_->GetVertexInputAttributeDescriptions();

    vk::PipelineVertexInputStateCreateInfo VertexInputInfo = {};
    VertexInputInfo   //
        .setVertexBindingDescriptions(BindingDesc)
        .setVertexAttributeDescriptions(AttributeDesc);

    // 配置输入装配
    vk::PipelineInputAssemblyStateCreateInfo InputAssemblyInfo = {};
    InputAssemblyInfo                                        //
        .setTopology(vk::PrimitiveTopology::eTriangleList)   // 每三个顶点构成一个图元
        .setPrimitiveRestartEnable(false);
    /************************* Shader配置结束 ************************/

    /************************* 配置视口 ************************/
    vk::Viewport viewport_info     = {};
    pipeline_info_.viewport.width  = pipeline_info_.viewport.width == 0
                                         ? context.GetSwapChainExtent().width
                                         : pipeline_info_.viewport.width;
    pipeline_info_.viewport.height = pipeline_info_.viewport.height == 0
                                         ? context.GetSwapChainExtent().height
                                         : pipeline_info_.viewport.height;
    viewport_info.x                = pipeline_info_.viewport.x;
    viewport_info.y                = pipeline_info_.viewport.y;
    viewport_info.width            = pipeline_info_.viewport.width;
    viewport_info.height           = pipeline_info_.viewport.height;
    viewport_info.minDepth         = 0.f;
    viewport_info.maxDepth         = 1.f;

    pipeline_info_.clipping_rect.width  = pipeline_info_.clipping_rect.width == 0
                                              ? context.GetSwapChainExtent().width
                                              : pipeline_info_.clipping_rect.width;
    pipeline_info_.clipping_rect.height = pipeline_info_.clipping_rect.height == 0
                                              ? context.GetSwapChainExtent().height
                                              : pipeline_info_.clipping_rect.height;
    vk::Rect2D scissor                  = {};
    scissor.offset.x                    = pipeline_info_.clipping_rect.offset_x;
    scissor.offset.y                    = pipeline_info_.clipping_rect.offset_y;
    scissor.extent.width                = pipeline_info_.clipping_rect.width;
    scissor.extent.height               = pipeline_info_.clipping_rect.height;

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
    vk::PipelineMultisampleStateCreateInfo MultisampleInfo = {};

    sample_count_ = pipeline_info_.multisample.sample_count;
    MultisampleInfo   //
        .setSampleShadingEnable(pipeline_info_.multisample.enabled)
        .setRasterizationSamples(pipeline_info_.multisample.sample_count);
    /************************* 配置多重采样结束 ************************/

    /************************* 配置深度模版测试 ************************/
    vk::PipelineDepthStencilStateCreateInfo DepthStencilInfo = {};
    DepthStencilInfo   //
        .setDepthTestEnable(pipeline_info_.depth_stencil_stage.enable_depth_test)
        .setDepthWriteEnable(pipeline_info_.depth_stencil_stage.enable_depth_write)
        .setDepthCompareOp(pipeline_info_.depth_stencil_stage.depth_compare_op)
        .setDepthBoundsTestEnable(pipeline_info_.depth_stencil_stage.enable_depth_bounds_test)
        .setStencilTestEnable(pipeline_info_.depth_stencil_stage.enable_stencil_test);
    /************************* 配置深度模版测试结束 ************************/

    /************************* 配置颜色混合 ************************/
    vk::PipelineColorBlendAttachmentState ColorBlendAttachment = {};
    ColorBlendAttachment   //
        .setColorWriteMask(
            vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
        )
        .setBlendEnable(pipeline_info_.color_blend_attachment_state.enabled);

    vk::PipelineColorBlendStateCreateInfo ColorBlendInfo = {};
    ColorBlendInfo   //
        .setLogicOpEnable(false)
        .setLogicOp(vk::LogicOp::eCopy)   // 逻辑操作
        .setAttachments({ColorBlendAttachment});
    /************************* 配置颜色混合结束 ************************/
    // 创建描述符集布局
    CreateDescriptionSetLayout();

    // 指定管线布局(uniform)
    vk::PipelineLayoutCreateInfo PipelineLayoutInfo = {};
    TStaticArray                 layout             = {shader_program_->GetDescriptorSetLayout()};
    PipelineLayoutInfo.setSetLayouts(layout);

    // 创建管线布局
    pipeline_layout_ = context.GetLogicalDevice()->CreatePipelineLayout(PipelineLayoutInfo);

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
    pipeline_create_info.setStages(ShaderStages)
        .setPVertexInputState(&VertexInputInfo)
        .setPInputAssemblyState(&InputAssemblyInfo)
        .setPViewportState(&viewport_state_create_info)
        .setPRasterizationState(&rasterizer_info)
        .setPMultisampleState(&MultisampleInfo)
        .setPDepthStencilState(&DepthStencilInfo)   // 深度和模版测试
        .setPColorBlendState(&ColorBlendInfo)       // 颜色混合
        .setLayout(pipeline_layout_)                // 管线布局
        .setRenderPass(render_pass_->GetHandle())   // RenderPass
        .setSubpass(0)                              // 子Pass
        .setPDynamicState(&dynamic_state_info);

    auto Pipeline = context.GetLogicalDevice()->GetHandle().createGraphicsPipeline(
        nullptr, pipeline_create_info
    );
    if (Pipeline.result != vk::Result::eSuccess)
    {
        throw VulkanException(L"创建管线失败");
    }
    else
    {
        pipeline_ = Pipeline.value;
    }
}

void GraphicsPipeline::CleanPipeline()
{
    const auto& Device = VulkanContext::Get().GetLogicalDevice();
    if (Device)
    {
        Device->GetHandle().destroyPipeline(pipeline_);
        Device->GetHandle().destroyPipelineLayout(pipeline_layout_);
        delete render_pass_;
        render_pass_     = nullptr;
        pipeline_        = nullptr;
        pipeline_layout_ = nullptr;
    }
}

void GraphicsPipeline::CreateOther(bool bRebuilding)
{
    CreateCommandBuffers();
}

void GraphicsPipeline::CleanOther(bool bRebuilding)
{
    // TODO: 模型系统解耦
    if (!bRebuilding)
    {
        CleanModel();
    }
}

void GraphicsPipeline::BeginRecordCommand(const vk::CommandBuffer InBuffer)
{
    vk::CommandBufferBeginInfo BeginInfo = {};
    BeginInfo.setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
    InBuffer.begin(BeginInfo);
}

void GraphicsPipeline::EndRecordCommand(const vk::CommandBuffer InBuffer)
{
    InBuffer.end();
}

void GraphicsPipeline::RecordCommand(
    vk::CommandBuffer InBuffer, const CommandRecordingParam& InParam
)
{
    VulkanContext&          Context        = VulkanContext::Get();
    vk::RenderPassBeginInfo RenderPassInfo = {};
    RenderPassInfo.setRenderPass(render_pass_->GetHandle())
        .setFramebuffer(InParam.frame_buffer)
        .setRenderArea({{0, 0}, Context.GetSwapChainExtent()});
    TStaticArray<vk::ClearValue, 2> ClearValues = {};
    ClearValues[0].color                        = vk::ClearColorValue{1.f, 0.f, 0.f, 1.f};
    ClearValues[1].depthStencil                 = vk::ClearDepthStencilValue{1.f, 0};
    RenderPassInfo.setClearValues(ClearValues);
    InBuffer.beginRenderPass(RenderPassInfo, vk::SubpassContents::eInline);
    InBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline_);
    if (pipeline_info_.dynamic_state_enabled & EPDSE_Viewport)
    {
        vk::Viewport NewViewport;
        NewViewport.x      = 0;
        NewViewport.y      = 0;
        NewViewport.width  = g_engine_statistics.window_size.width;
        NewViewport.height = g_engine_statistics.window_size.height;
        InBuffer.setViewport(0, NewViewport);
    }
    if (pipeline_info_.dynamic_state_enabled & EPDSE_Scissor)
    {
        vk::Rect2D NewScissor;
        NewScissor.offset.x      = 0;
        NewScissor.offset.y      = 0;
        NewScissor.extent.width  = g_engine_statistics.window_size.width;
        NewScissor.extent.height = g_engine_statistics.window_size.height;
        InBuffer.setScissor(0, NewScissor);
    }
    // 使用顶点缓冲需纳入

    int i = 0;
    for (const auto& Mesh: model->GetMeshes())
    {
        // 绑定顶点缓冲
        i++;
        vk::DeviceSize Offset          = 0;
        vk::Buffer     VertexBuffers[] = {Mesh->GetVertexBuffer()};
        InBuffer.bindVertexBuffers(0, 1, VertexBuffers, &Offset);
        InBuffer.bindIndexBuffer(Mesh->GetIndexBuffer(), 0, vk::IndexType::eUint32);
        // 绑定描述符集
        TArray Set = {InParam.descriptor_set};
        InBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline_layout_, 0, Set, {});
        InBuffer.drawIndexed(static_cast<UInt32>(Mesh->GetIndexCount()), 1, 0, 0, 0);
    }
    InBuffer.endRenderPass();
}

void GraphicsPipeline::LoadModel()
{
    VulkanContext& Context       = VulkanContext::Get();
    auto           ModelResource = Resource::Model::Create(L"Models/AK47/AK47_CS2.fbx");
    model                        = Model::CreateUnique(ModelResource, Context);
}

void GraphicsPipeline::CleanModel() const
{
    model->Finialize();
}

void GraphicsPipeline::CreateCommandBuffers()
{
    VulkanContext&                context    = VulkanContext::Get();
    vk::CommandBufferAllocateInfo alloc_info = {};
    alloc_info.setCommandPool(context.GetCommandPool()->GetCommandPool())
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandBufferCount(context.GetSwapChainImageCount());

    command_buffers_ = context.GetCommandPool()->CreateCommandBuffers(alloc_info);

    for (size_t i = 0; i < command_buffers_.size(); i++)
    {
        auto command_buffer = command_buffers_[i];
        BeginRecordCommand(command_buffer);
        {
            CommandRecordingParam param;
            param.frame_buffer   = render_pass_->GetFrameBuffer(i)->GetHandle();
            param.descriptor_set = descriptor_sets_[i];
            RecordCommand(command_buffer, param);
        }
        EndRecordCommand(command_buffer);
    }
}

void GraphicsPipeline::CleanCommandBuffers() const
{
    VulkanContext& Context = VulkanContext::Get();
    Context.GetCommandPool()->DestroyCommandBuffers(command_buffers_);
}

void GraphicsPipeline::SubmitGraphicsQueue(
    int CurrentImageIndex, vk::Queue InGraphicsQueue, TArray<vk::Semaphore> InWaitSemaphores,
    TArray<vk::Semaphore> InSingalSemaphores, vk::Fence InFrameFence
)
{
    auto                   CmdBuffer  = GetCurrentImageCommandBuffer(CurrentImageIndex);
    vk::SubmitInfo         SubmitInfo = {};
    vk::PipelineStageFlags WaitFlag   = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    SubmitInfo.setCommandBuffers(CmdBuffer)
        .setWaitSemaphores(InWaitSemaphores)
        .setSignalSemaphores(InSingalSemaphores)
        .setWaitDstStageMask(WaitFlag);
    InGraphicsQueue.submit(SubmitInfo, InFrameFence);
}

void GraphicsPipeline::Rebuild()
{
    CleanOther(true);
    if (!(pipeline_info_.dynamic_state_enabled | EPDSE_None)) {
        // 启用了DynamicState则通过DynamicState设置
        CleanPipeline();
        CreatePipeline();
    }
    CreateOther(true);
}

RHI_VULKAN_NAMESPACE_END
