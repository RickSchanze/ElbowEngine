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
#include "RHI/Vulkan/Resource/Image.h"
#include "RHI/Vulkan/Resource/ImageView.h"
#include "RHI/Vulkan/Resource/VulkanModel.h"
#include "RHI/Vulkan/VulkanContext.h"
#include "Shader.h"
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

GraphicsPipeline::GraphicsPipeline(const PipelineInitializer& InInitializer)
{
    VulkanContext& Context = VulkanContext::Get();
    pipeline_info_         = InInitializer;
    CreatePipeline();
    CreateOther(false);
    Context.AddPipelineToRender(this);
    LOG_INFO_CATEGORY(Vulkan, L"图形管线初始化完成");
}

void GraphicsPipeline::UpdateUniformBuffer(const UInt32 InCurrentImage) const
{
    VulkanContext& Context     = VulkanContext::Get();
    static auto    StartTime   = std::chrono::high_resolution_clock::now();
    const auto     CurrentTime = std::chrono::high_resolution_clock::now();
    const float    Time =
        std::chrono::duration<float, std::chrono::seconds::period>(CurrentTime - StartTime).count();
    TStaticArray<glm::mat4, 3> UBO;
    UBO[0] = glm::mat4(1.f);
    // TODO: Here
    UBO[0] = rotate(UBO[0], glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
    // 缩放
    UBO[0] = scale(UBO[0], glm::vec3(0.05f, 0.05f, 0.05f));
    // 绕X转90度
    UBO[0] = rotate(UBO[0], glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
    UBO[1] = Function::Camera::Main->GetViewMatrix();

    UBO[2] = glm::perspective(
        glm::radians(45.f),
        static_cast<float>(Context.GetSwapChainExtent().width) /
            static_cast<float>(Context.GetSwapChainExtent().height),
        0.1f,
        10.f
    );
    UBO[2][1][1] *= -1;
    void* Data;
    Context.GetLogicalDevice()->MapMemory(
        uniform_buffers_memory_[InCurrentImage], 3 * sizeof(glm::mat4), 0, &Data
    );
    memcpy(Data, UBO.data(), 3 * sizeof(glm::mat4));
    Context.GetLogicalDevice()->UnmapMemory(uniform_buffers_memory_[InCurrentImage]);
}

vk::CommandBuffer GraphicsPipeline::GetCurrentImageCommandBuffer(const UInt32 InCurrentImage) const
{
    return command_buffers_[InCurrentImage];
}

void GraphicsPipeline::CreatePipeline()
{
    VulkanContext& Context = VulkanContext::Get();
    /************************* 配置RenderPass ************************/
    if (pipeline_info_.RenderPass == nullptr)
    {
        // RenderPass或许可以共用
        // @TODO: 探索RenderPass共用
        render_pass_              = new RenderPass();
        render_pass_->SampleCount = pipeline_info_.Multisample.SampleCount;
    }
    render_pass_->Initialize();
    /************************* 配置RenderPass结束 ************************/

    /************************* 配置Shader ************************/
    // @TODO: 多Shader共用
    const auto VertShader =
        Shader::CreateShared(pipeline_info_.ShaderStage.VertexShaderPath, EShaderStage::Vertex);
    const auto FragShader =
        Shader::CreateShared(pipeline_info_.ShaderStage.FragmentShaderPath, EShaderStage::Fragment);
    m_shader_prog_ = ShaderProgram::CreateShared(VertShader, FragShader);

    // 配置Shader的阶段
    vk::PipelineShaderStageCreateInfo VertInfo{};
    VertInfo.setStage(vk::ShaderStageFlagBits::eVertex)
        .setModule(VertShader->GetShaderModule())
        .setPName("main");
    vk::PipelineShaderStageCreateInfo FragInfo{};
    FragInfo.setStage(vk::ShaderStageFlagBits::eFragment)
        .setModule(FragShader->GetShaderModule())
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
    vk::Viewport ViewportInfo      = {};
    pipeline_info_.Viewport.Width  = pipeline_info_.Viewport.Width == 0
                                         ? Context.GetSwapChainExtent().width
                                         : pipeline_info_.Viewport.Width;
    pipeline_info_.Viewport.Height = pipeline_info_.Viewport.Height == 0
                                         ? Context.GetSwapChainExtent().height
                                         : pipeline_info_.Viewport.Height;
    ViewportInfo.x                 = pipeline_info_.Viewport.X;
    ViewportInfo.y                 = pipeline_info_.Viewport.Y;
    ViewportInfo.width             = pipeline_info_.Viewport.Width;
    ViewportInfo.height            = pipeline_info_.Viewport.Height;
    ViewportInfo.minDepth          = 0.f;
    ViewportInfo.maxDepth          = 1.f;

    pipeline_info_.ClippingRect.Width  = pipeline_info_.ClippingRect.Width == 0
                                             ? Context.GetSwapChainExtent().width
                                             : pipeline_info_.ClippingRect.Width;
    pipeline_info_.ClippingRect.Height = pipeline_info_.ClippingRect.Height == 0
                                             ? Context.GetSwapChainExtent().height
                                             : pipeline_info_.ClippingRect.Height;
    vk::Rect2D Scissor                 = {};
    Scissor.offset.x                   = pipeline_info_.ClippingRect.OffsetX;
    Scissor.offset.y                   = pipeline_info_.ClippingRect.OffsetY;
    Scissor.extent.width               = pipeline_info_.ClippingRect.Width;
    Scissor.extent.height              = pipeline_info_.ClippingRect.Height;

    vk::PipelineViewportStateCreateInfo ViewportStateCreateInfo = {};
    ViewportStateCreateInfo.setViewports({ViewportInfo}).setScissors({Scissor});
    /************************* 配置视口结束 ************************/

    /************************* 配置光栅化 ************************/
    // 光栅化设置 将顶点着色器构成的几何图形转化为片段交给片段着色器着色
    vk::PipelineRasterizationStateCreateInfo RasterizerInfo = {};
    // clang-format off
    RasterizerInfo
        .setDepthClampEnable(pipeline_info_.RasterizationStage.bEnableDepthClamp)
        .setRasterizerDiscardEnable(pipeline_info_.RasterizationStage.bEnableRaterizerDiscard)
        .setPolygonMode(pipeline_info_.RasterizationStage.PolygonMode)
        .setLineWidth(pipeline_info_.RasterizationStage.LineWidth)
        .setCullMode(pipeline_info_.RasterizationStage.CullMode)
        .setFrontFace(pipeline_info_.RasterizationStage.FrontFace)
        .setDepthBiasEnable(pipeline_info_.RasterizationStage.bEnableDepthBias);
    // clang-format on
    /************************* 配置光栅化结束 ************************/

    /************************* 配置多重采样 ************************/
    vk::PipelineMultisampleStateCreateInfo MultisampleInfo = {};

    sample_count_ = pipeline_info_.Multisample.SampleCount;
    MultisampleInfo   //
        .setSampleShadingEnable(pipeline_info_.Multisample.bEnable)
        .setRasterizationSamples(pipeline_info_.Multisample.SampleCount);
    /************************* 配置多重采样结束 ************************/

    /************************* 配置深度模版测试 ************************/
    vk::PipelineDepthStencilStateCreateInfo DepthStencilInfo = {};
    DepthStencilInfo   //
        .setDepthTestEnable(pipeline_info_.DepthStencilStage.bEnableDepthTest)
        .setDepthWriteEnable(pipeline_info_.DepthStencilStage.bEnableDepthWrite)
        .setDepthCompareOp(pipeline_info_.DepthStencilStage.DepthCompareOp)
        .setDepthBoundsTestEnable(pipeline_info_.DepthStencilStage.bEnableDepthBoundsTest)
        .setStencilTestEnable(pipeline_info_.DepthStencilStage.bEnableStencilTest);
    /************************* 配置深度模版测试结束 ************************/

    /************************* 配置颜色混合 ************************/
    vk::PipelineColorBlendAttachmentState ColorBlendAttachment = {};
    ColorBlendAttachment   //
        .setColorWriteMask(
            vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
        )
        .setBlendEnable(pipeline_info_.ColorBlendAttachmentState.bEnable);

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
    PipelineLayoutInfo.setSetLayouts({descriptor_set_layout_});

    // 创建管线布局
    pipeline_layout_ =
        Context.GetLogicalDevice()->GetHandle().createPipelineLayout(PipelineLayoutInfo);

    // DynamicState
    vk::PipelineDynamicStateCreateInfo DynamicStateInfo;
    TArray<vk::DynamicState>           DynamicStates;
    if (!(pipeline_info_.DynamicStateEnabled & EPDSE_None))
    {
        if (pipeline_info_.DynamicStateEnabled & EPDSE_Scissor)
        {
            DynamicStates.emplace_back(vk::DynamicState::eScissor);
        }
        if (pipeline_info_.DynamicStateEnabled & EPDSE_Viewport)
        {
            DynamicStates.emplace_back(vk::DynamicState::eViewport);
        }
    }
    DynamicStateInfo.setDynamicStates(DynamicStates);

    // 定义管线
    vk::GraphicsPipelineCreateInfo PipelineInfo = {};
    PipelineInfo.setStages(ShaderStages)
        .setPVertexInputState(&VertexInputInfo)
        .setPInputAssemblyState(&InputAssemblyInfo)
        .setPViewportState(&ViewportStateCreateInfo)
        .setPRasterizationState(&RasterizerInfo)
        .setPMultisampleState(&MultisampleInfo)
        .setPDepthStencilState(&DepthStencilInfo)   // 深度和模版测试
        .setPColorBlendState(&ColorBlendInfo)       // 颜色混合
        .setLayout(pipeline_layout_)                // 管线布局
        .setRenderPass(render_pass_->GetHandle())   // RenderPass
        .setSubpass(0)                              // 子Pass
        .setPDynamicState(&DynamicStateInfo);

    auto Pipeline =
        Context.GetLogicalDevice()->GetHandle().createGraphicsPipeline(nullptr, PipelineInfo);
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
        render_pass_           = nullptr;
        pipeline_              = nullptr;
        pipeline_layout_       = nullptr;
    }
}

void GraphicsPipeline::CreateOther(bool bRebuilding)
{
    // TODO: 重构并整个至材质系统
    CreateUniformBuffers();
    CreateDescriptorPool();
    CreateDescriptotSets();
    // TODO: 模型系统解耦
    if (!bRebuilding)
    {
        LoadModel();
    }
    CreateCommandBuffers();
}

void GraphicsPipeline::CleanOther(bool bRebuilding)
{
    CleanDescriptorPool();
    // TODO: 模型系统解耦
    if (!bRebuilding)
    {
        CleanModel();
    }
    // TODO: 重构并整合至材质系统
    // 清理纹理
    CleanUniformBuffers();
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
    if (pipeline_info_.DynamicStateEnabled & EPDSE_Viewport)
    {
        vk::Viewport NewViewport;
        NewViewport.x      = 0;
        NewViewport.y      = 0;
        NewViewport.width  = g_engine_statistics.window_size.width;
        NewViewport.height = g_engine_statistics.window_size.height;
        InBuffer.setViewport(0, NewViewport);
    }
    if (pipeline_info_.DynamicStateEnabled & EPDSE_Scissor)
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

void GraphicsPipeline::CreateDescriptionSetLayout()
{
    VulkanContext&                         Context = VulkanContext::Get();
    TArray<vk::DescriptorSetLayoutBinding> UniformBindings;
    for (const auto& UniformBinding: m_shader_prog_->GetUniforms() | std::views::values)
    {
        vk::DescriptorSetLayoutBinding binding;
        binding.binding         = UniformBinding.binding;
        binding.descriptorType  = GetVkDescriptorType(UniformBinding.type);
        binding.descriptorCount = 1;
        binding.stageFlags      = GetVkShaderStage(UniformBinding.stage);
        UniformBindings.push_back(binding);
    }
    vk::DescriptorSetLayoutCreateInfo LayoutInfo{};
    LayoutInfo.setBindings(UniformBindings);
    descriptor_set_layout_ =
        Context.GetLogicalDevice()->GetHandle().createDescriptorSetLayout(LayoutInfo);
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
    VulkanContext&                context   = VulkanContext::Get();
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
    if (!(pipeline_info_.DynamicStateEnabled | EPDSE_None)) {
        // 启用了DynamicState则通过DynamicState设置
        CleanPipeline();
        CreatePipeline();
    }
    CreateOther(true);
}

RHI_VULKAN_NAMESPACE_END
