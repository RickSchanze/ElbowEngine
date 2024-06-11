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
#include "Model.h"
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
    mPipelineInfo          = InInitializer;
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
        mUniformBuffersMemory[InCurrentImage], 3 * sizeof(glm::mat4), 0, &Data
    );
    memcpy(Data, UBO.data(), 3 * sizeof(glm::mat4));
    Context.GetLogicalDevice()->UnmapMemory(mUniformBuffersMemory[InCurrentImage]);
}

vk::CommandBuffer GraphicsPipeline::GetCurrentImageCommandBuffer(const UInt32 InCurrentImage) const
{
    return mCommandBuffers[InCurrentImage];
}

void GraphicsPipeline::CreatePipeline()
{
    VulkanContext& Context = VulkanContext::Get();
    /************************* 配置RenderPass ************************/
    if (mPipelineInfo.RenderPass == nullptr)
    {
        // RenderPass或许可以共用
        // @TODO: 探索RenderPass共用
        mRenderPass              = new RenderPass();
        mRenderPass->SampleCount = mPipelineInfo.Multisample.SampleCount;
    }
    mRenderPass->Initialize();
    /************************* 配置RenderPass结束 ************************/

    /************************* 配置Shader ************************/
    // @TODO: 多Shader共用
    const auto VertShader =
        Shader::CreateShared(mPipelineInfo.ShaderStage.VertexShaderPath, EShaderStage::Vertex);
    const auto FragShader =
        Shader::CreateShared(mPipelineInfo.ShaderStage.FragmentShaderPath, EShaderStage::Fragment);
    mShaderProg = ShaderProgram::CreateShared(VertShader, FragShader);

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
    auto BindingDesc   = mShaderProg->GetVertexInputBindingDescription();
    auto AttributeDesc = mShaderProg->GetVertexInputAttributeDescriptions();

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
    vk::Viewport ViewportInfo     = {};
    mPipelineInfo.Viewport.Width  = mPipelineInfo.Viewport.Width == 0
                                        ? Context.GetSwapChainExtent().width
                                        : mPipelineInfo.Viewport.Width;
    mPipelineInfo.Viewport.Height = mPipelineInfo.Viewport.Height == 0
                                        ? Context.GetSwapChainExtent().height
                                        : mPipelineInfo.Viewport.Height;
    ViewportInfo.x                = mPipelineInfo.Viewport.X;
    ViewportInfo.y                = mPipelineInfo.Viewport.Y;
    ViewportInfo.width            = mPipelineInfo.Viewport.Width;
    ViewportInfo.height           = mPipelineInfo.Viewport.Height;
    ViewportInfo.minDepth         = 0.f;
    ViewportInfo.maxDepth         = 1.f;

    mPipelineInfo.ClippingRect.Width  = mPipelineInfo.ClippingRect.Width == 0
                                            ? Context.GetSwapChainExtent().width
                                            : mPipelineInfo.ClippingRect.Width;
    mPipelineInfo.ClippingRect.Height = mPipelineInfo.ClippingRect.Height == 0
                                            ? Context.GetSwapChainExtent().height
                                            : mPipelineInfo.ClippingRect.Height;
    vk::Rect2D Scissor                = {};
    Scissor.offset.x                  = mPipelineInfo.ClippingRect.OffsetX;
    Scissor.offset.y                  = mPipelineInfo.ClippingRect.OffsetY;
    Scissor.extent.width              = mPipelineInfo.ClippingRect.Width;
    Scissor.extent.height             = mPipelineInfo.ClippingRect.Height;

    vk::PipelineViewportStateCreateInfo ViewportStateCreateInfo = {};
    ViewportStateCreateInfo.setViewports({ViewportInfo}).setScissors({Scissor});
    /************************* 配置视口结束 ************************/

    /************************* 配置光栅化 ************************/
    // 光栅化设置 将顶点着色器构成的几何图形转化为片段交给片段着色器着色
    vk::PipelineRasterizationStateCreateInfo RasterizerInfo = {};
    // clang-format off
    RasterizerInfo
        .setDepthClampEnable(mPipelineInfo.RasterizationStage.bEnableDepthClamp)
        .setRasterizerDiscardEnable(mPipelineInfo.RasterizationStage.bEnableRaterizerDiscard)
        .setPolygonMode(mPipelineInfo.RasterizationStage.PolygonMode)
        .setLineWidth(mPipelineInfo.RasterizationStage.LineWidth)
        .setCullMode(mPipelineInfo.RasterizationStage.CullMode)
        .setFrontFace(mPipelineInfo.RasterizationStage.FrontFace)
        .setDepthBiasEnable(mPipelineInfo.RasterizationStage.bEnableDepthBias);
    // clang-format on
    /************************* 配置光栅化结束 ************************/

    /************************* 配置多重采样 ************************/
    vk::PipelineMultisampleStateCreateInfo MultisampleInfo = {};

    mSampleCount = mPipelineInfo.Multisample.SampleCount;
    MultisampleInfo   //
        .setSampleShadingEnable(mPipelineInfo.Multisample.bEnable)
        .setRasterizationSamples(mPipelineInfo.Multisample.SampleCount);
    /************************* 配置多重采样结束 ************************/

    /************************* 配置深度模版测试 ************************/
    vk::PipelineDepthStencilStateCreateInfo DepthStencilInfo = {};
    DepthStencilInfo   //
        .setDepthTestEnable(mPipelineInfo.DepthStencilStage.bEnableDepthTest)
        .setDepthWriteEnable(mPipelineInfo.DepthStencilStage.bEnableDepthWrite)
        .setDepthCompareOp(mPipelineInfo.DepthStencilStage.DepthCompareOp)
        .setDepthBoundsTestEnable(mPipelineInfo.DepthStencilStage.bEnableDepthBoundsTest)
        .setStencilTestEnable(mPipelineInfo.DepthStencilStage.bEnableStencilTest);
    /************************* 配置深度模版测试结束 ************************/

    /************************* 配置颜色混合 ************************/
    vk::PipelineColorBlendAttachmentState ColorBlendAttachment = {};
    ColorBlendAttachment   //
        .setColorWriteMask(
            vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
        )
        .setBlendEnable(mPipelineInfo.ColorBlendAttachmentState.bEnable);

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
    PipelineLayoutInfo.setSetLayouts({mDescriptorSetLayout});

    // 创建管线布局
    mPipelineLayout =
        Context.GetLogicalDevice()->GetHandle().createPipelineLayout(PipelineLayoutInfo);

    // DynamicState
    vk::PipelineDynamicStateCreateInfo DynamicStateInfo;
    TArray<vk::DynamicState>           DynamicStates;
    if (!(mPipelineInfo.DynamicStateEnabled & EPDSE_None))
    {
        if (mPipelineInfo.DynamicStateEnabled & EPDSE_Scissor)
        {
            DynamicStates.emplace_back(vk::DynamicState::eScissor);
        }
        if (mPipelineInfo.DynamicStateEnabled & EPDSE_Viewport)
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
        .setLayout(mPipelineLayout)                 // 管线布局
        .setRenderPass(mRenderPass->GetHandle())    // RenderPass
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
        mPipeline = Pipeline.value;
    }
}

void GraphicsPipeline::CleanPipeline()
{
    const auto& Device = VulkanContext::Get().GetLogicalDevice();
    if (Device)
    {
        Device->GetHandle().destroyPipeline(mPipeline);
        Device->GetHandle().destroyPipelineLayout(mPipelineLayout);
        Device->GetHandle().destroyDescriptorSetLayout(mDescriptorSetLayout);
        delete mRenderPass;
        mRenderPass          = nullptr;
        mPipeline            = nullptr;
        mPipelineLayout      = nullptr;
        mDescriptorSetLayout = nullptr;
    }
}

void GraphicsPipeline::CreateOther(bool bRebuilding)
{
    // TODO: 重构并整个至材质系统
    CreateTextureImageAndView();
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
    CleanTextureImageAndView();
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
    RenderPassInfo.setRenderPass(mRenderPass->GetHandle())
        .setFramebuffer(InParam.FrameBuffer)
        .setRenderArea({{0, 0}, Context.GetSwapChainExtent()});
    TStaticArray<vk::ClearValue, 2> ClearValues = {};
    ClearValues[0].color                        = vk::ClearColorValue{1.f, 0.f, 0.f, 1.f};
    ClearValues[1].depthStencil                 = vk::ClearDepthStencilValue{1.f, 0};
    RenderPassInfo.setClearValues(ClearValues);
    InBuffer.beginRenderPass(RenderPassInfo, vk::SubpassContents::eInline);
    InBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);
    if (mPipelineInfo.DynamicStateEnabled & EPDSE_Viewport)
    {
        vk::Viewport NewViewport;
        NewViewport.x      = 0;
        NewViewport.y      = 0;
        NewViewport.width  = gEngineStatistics.WindowSize.Width;
        NewViewport.height = gEngineStatistics.WindowSize.Height;
        InBuffer.setViewport(0, NewViewport);
    }
    if (mPipelineInfo.DynamicStateEnabled & EPDSE_Scissor)
    {
        vk::Rect2D NewScissor;
        NewScissor.offset.x      = 0;
        NewScissor.offset.y      = 0;
        NewScissor.extent.width  = gEngineStatistics.WindowSize.Width;
        NewScissor.extent.height = gEngineStatistics.WindowSize.Height;
        InBuffer.setScissor(0, NewScissor);
    }
    // 使用顶点缓冲需纳入

    int i = 0;
    for (const auto& Mesh: mModel->GetMeshes())
    {
        // 绑定顶点缓冲
        i++;
        vk::DeviceSize Offset          = 0;
        vk::Buffer     VertexBuffers[] = {Mesh->GetVertexBuffer()};
        InBuffer.bindVertexBuffers(0, 1, VertexBuffers, &Offset);
        InBuffer.bindIndexBuffer(Mesh->GetIndexBuffer(), 0, vk::IndexType::eUint32);
        // 绑定描述符集
        TArray Set = {InParam.DescriptorSet};
        InBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mPipelineLayout, 0, Set, {});
        InBuffer.drawIndexed(static_cast<UInt32>(Mesh->GetIndexCount()), 1, 0, 0, 0);
    }
    InBuffer.endRenderPass();
}

void GraphicsPipeline::CreateDescriptionSetLayout()
{
    VulkanContext&                         Context = VulkanContext::Get();
    TArray<vk::DescriptorSetLayoutBinding> UniformBindings;
    for (const auto& UniformBinding: mShaderProg->GetUniforms() | std::views::values)
    {
        vk::DescriptorSetLayoutBinding Binding;
        Binding.binding         = UniformBinding.Binding;
        Binding.descriptorType  = GetVkDescriptorType(UniformBinding.Type);
        Binding.descriptorCount = 1;
        Binding.stageFlags      = GetVkShaderStage(UniformBinding.Stage);
        UniformBindings.push_back(Binding);
    }
    vk::DescriptorSetLayoutCreateInfo LayoutInfo{};
    LayoutInfo.setBindings(UniformBindings);
    mDescriptorSetLayout =
        Context.GetLogicalDevice()->GetHandle().createDescriptorSetLayout(LayoutInfo);
}

void GraphicsPipeline::LoadModel()
{
    VulkanContext& Context       = VulkanContext::Get();
    auto           ModelResource = Resource::Model::Create(L"Models/AK47/AK47_CS2.fbx");
    mModel                       = Model::CreateUnique(ModelResource, Context);
}

void GraphicsPipeline::CleanModel() const
{
    mModel->Finialize();
}

void GraphicsPipeline::CreateUniformBuffers()
{
    VulkanContext& Context    = VulkanContext::Get();
    vk::DeviceSize BufferSize = 0;
    for (const auto& [Key, Value]: mShaderProg->GetUniforms())
    {
        BufferSize += Value.Size;
    }
    mUniformBuffers.resize(Context.GetSwapChainImageCount());
    mUniformBuffersMemory.resize(Context.GetSwapChainImageCount());
    for (size_t i = 0; i < Context.GetSwapChainImageCount(); i++)
    {
        Context.GetLogicalDevice()->CreateBuffer(
            BufferSize,
            vk::BufferUsageFlagBits::eUniformBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            mUniformBuffers[i],
            mUniformBuffersMemory[i]
        );
    }
}

void GraphicsPipeline::CleanUniformBuffers()
{
    VulkanContext& Context = VulkanContext::Get();
    for (size_t i = 0; i < Context.GetSwapChainImageCount(); i++)
    {
        Context.GetLogicalDevice()->GetHandle().destroyBuffer(mUniformBuffers[i]);
        Context.GetLogicalDevice()->GetHandle().freeMemory(mUniformBuffersMemory[i]);
    }
    mUniformBuffers.clear();
    mUniformBuffersMemory.clear();
}

void GraphicsPipeline::CreateDescriptorPool()
{
    VulkanContext&                          Context   = VulkanContext::Get();
    TStaticArray<vk::DescriptorPoolSize, 2> PoolSizes = {};
    // Uniform Object
    PoolSizes[0].type                                 = vk::DescriptorType::eUniformBuffer;
    PoolSizes[0].descriptorCount                      = Context.GetSwapChainImageCount();

    // 纹理采样器
    PoolSizes[1].type            = vk::DescriptorType::eCombinedImageSampler;
    PoolSizes[1].descriptorCount = Context.GetSwapChainImageCount();

    vk::DescriptorPoolCreateInfo PoolInfo = {};
    PoolInfo.setPoolSizes(PoolSizes).setMaxSets(Context.GetSwapChainImageCount());
    mDescriptorPool = Context.GetLogicalDevice()->GetHandle().createDescriptorPool(PoolInfo);
}

void GraphicsPipeline::CleanDescriptorPool() const
{
    VulkanContext& Context = VulkanContext::Get();
    Context.GetLogicalDevice()->GetHandle().destroyDescriptorPool(mDescriptorPool);
}

void GraphicsPipeline::CreateDescriptotSets()
{
    VulkanContext&                  Context = VulkanContext::Get();
    TArray<vk::DescriptorSetLayout> Layouts(Context.GetSwapChainImageCount(), mDescriptorSetLayout);
    vk::DescriptorSetAllocateInfo   AllocInfo = {};
    AllocInfo.setDescriptorPool(mDescriptorPool).setSetLayouts(Layouts);
    mDescriptorSets.resize(Context.GetSwapChainImageCount());
    // 描述符池对象销毁时会自动清除描述符集
    mDescriptorSets = Context.GetLogicalDevice()->GetHandle().allocateDescriptorSets(AllocInfo);
    for (size_t i = 0; i < mDescriptorSets.size(); i++)
    {
        vk::DescriptorBufferInfo BufferInfo = {};
        BufferInfo.setBuffer(mUniformBuffers[i]).setOffset(0).setRange(VK_WHOLE_SIZE);
        vk::DescriptorImageInfo ImageInfo = {};
        ImageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
            .setImageView(mTextureView->GetHandle())
            .setSampler(mTextureSampler);

        TStaticArray<vk::WriteDescriptorSet, 2> DescriptorWrites = {};

        vk::WriteDescriptorSet DescriptorWrite = {};
        DescriptorWrite.setDstSet(mDescriptorSets[i])
            .setDstBinding(0)
            .setDstArrayElement(0)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setDescriptorCount(1)
            .setPBufferInfo(&BufferInfo);
        DescriptorWrites[0]                 = DescriptorWrite;
        DescriptorWrites[1].dstSet          = mDescriptorSets[i];
        DescriptorWrites[1].dstBinding      = 1;
        DescriptorWrites[1].dstArrayElement = 0;
        DescriptorWrites[1].descriptorType  = vk::DescriptorType::eCombinedImageSampler;
        DescriptorWrites[1].descriptorCount = 1;
        DescriptorWrites[1].pImageInfo      = &ImageInfo;
        Context.GetLogicalDevice()->GetHandle().updateDescriptorSets(DescriptorWrites, nullptr);
    }
}

void GraphicsPipeline::CreateCommandBuffers()
{
    VulkanContext&                Context   = VulkanContext::Get();
    vk::CommandBufferAllocateInfo AllocInfo = {};
    AllocInfo.setCommandPool(Context.GetCommandPool()->GetCommandPool())
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandBufferCount(Context.GetSwapChainImageCount());

    mCommandBuffers = Context.GetCommandPool()->CreateCommandBuffers(AllocInfo);

    for (size_t i = 0; i < mCommandBuffers.size(); i++)
    {
        auto CommandBuffer = mCommandBuffers[i];
        BeginRecordCommand(CommandBuffer);
        {
            CommandRecordingParam Param;
            Param.FrameBuffer   = mRenderPass->GetFrameBuffer(i)->GetHandle();
            Param.DescriptorSet = mDescriptorSets[i];
            RecordCommand(CommandBuffer, Param);
        }
        EndRecordCommand(CommandBuffer);
    }
}

void GraphicsPipeline::CleanCommandBuffers() const
{
    VulkanContext& Context = VulkanContext::Get();
    Context.GetCommandPool()->DestroyCommandBuffers(mCommandBuffers);
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
    if (!(mPipelineInfo.DynamicStateEnabled | EPDSE_None)) {
        // 启用了DynamicState则通过DynamicState设置
        CleanPipeline();
        CreatePipeline();
    }
    CreateOther(true);
}

RHI_VULKAN_NAMESPACE_END
