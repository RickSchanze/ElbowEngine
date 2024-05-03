/**
 * @file GraphicsPipeline.cpp
 * @author Echo 
 * @Date 24-4-27
 * @brief 
 */

#include "GraphicsPipeline.h"

#include "CommandProducer.h"
#include "CoreGlobal.h"
#include "LogicalDevice.h"
#include "RenderPass.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "VulkanRenderer.h"

#include <ranges>

RHI_VULKAN_NAMESPACE_BEGIN

GraphicsPipeline::GraphicsPipeline(Ref<VulkanRenderer> InRenderer, const GraphicsPipelineCreateInfo& InCreateInfo) : mRenderer(InRenderer) {
    // 创建RenderPass
    if (!InCreateInfo.RenderPassProducer) {
        throw VulkanException(L"创建图形管线失败：RenderPassProducer为空");
    }
    auto&      Device             = InRenderer.get().GetLogicalDevice();
    const auto RenderPassProducer = InCreateInfo.RenderPassProducer->GetRenderPassCreateInfo();
    mRenderPass                   = RenderPass::CreateUnique(MakeRef(*Device), RenderPassProducer);

    const auto VertShader = Shader::CreateShared(MakeRef(*Device), InCreateInfo.VertexShaderPath, EShaderStage::Vertex);
    const auto FragShader = Shader::CreateShared(MakeRef(*Device), InCreateInfo.FragmentShaderPath, EShaderStage::Fragment);
    mShaderProg           = ShaderProgram::CreateShared(MakeRef(*Device), VertShader, FragShader);

    // 配置Shader的阶段
    vk::PipelineShaderStageCreateInfo VertInfo{};
    VertInfo.setStage(vk::ShaderStageFlagBits::eVertex).setModule(VertShader->GetShaderModule()).setPName("main");
    vk::PipelineShaderStageCreateInfo FragInfo{};
    FragInfo.setStage(vk::ShaderStageFlagBits::eFragment).setModule(FragShader->GetShaderModule()).setPName("main");
    StaticArray<vk::PipelineShaderStageCreateInfo, 2> ShaderStages = {VertInfo, FragInfo};

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

    if (InCreateInfo.ViewportSize.height == 0 || InCreateInfo.ViewportSize.width == 0) {
        throw VulkanException(L"创建图形管线失败：视口大小不合法");
    }
    // 设置视口
    vk::Viewport ViewportInfo = {};
    ViewportInfo   //
        .setX(0)
        .setY(0)   // xy是左上角
        .setWidth(static_cast<float>(InCreateInfo.ViewportSize.width))
        .setHeight(static_cast<float>(InCreateInfo.ViewportSize.height))
        .setMinDepth(0.f)
        .setMaxDepth(1.f);   //深度的最大最小值必须在[0,1]职中

    // 裁剪矩形
    vk::Rect2D Scissor = {};
    Scissor   //
        .setOffset({0, 0})
        .setExtent(InCreateInfo.ViewportSize);

    vk::PipelineViewportStateCreateInfo ViewportStateCreateInfo = {};
    ViewportStateCreateInfo.setViewports({ViewportInfo}).setScissors({Scissor});

    // 光栅化设置 将顶点着色器构成的几何图形转化为片段交给片段着色器着色
    vk::PipelineRasterizationStateCreateInfo RasterizerInfo = {};
    // clang-format off
    RasterizerInfo
        .setDepthClampEnable(false)                     // 是否开启深度裁剪,如果为true则近平面和远平面之外的片段会被裁剪到近平面和远平面 对阴影贴图的生成很有用
        .setRasterizerDiscardEnable(false)              // 是否开启丢弃光栅化 若为true则所有几何图元都不能通过光栅化阶段 会禁止一切片段输出到帧缓冲 且需要启用相应特性
        .setPolygonMode(vk::PolygonMode::eFill)         // 多边形模式(多边形包括多边形内部都产生片段)
        .setLineWidth(1.f)
        .setCullMode(vk::CullModeFlagBits::eBack)       // 设置为背面剔除
        .setFrontFace(vk::FrontFace::eCounterClockwise) // 逆时针为正面
        .setDepthBiasEnable(false);                     // 是否开启深度偏移
    // clang-format on

    // 多重采样设置
    vk::PipelineMultisampleStateCreateInfo MultisampleInfo = {};
    mMsaaSamples                                           = InCreateInfo.MsaaSamples;
    MultisampleInfo   //
        .setSampleShadingEnable(false)
        .setRasterizationSamples(mMsaaSamples);

    // 配置深度模版测试阶段
    vk::PipelineDepthStencilStateCreateInfo DepthStencilInfo = {};
    DepthStencilInfo   //
        .setDepthTestEnable(true)
        .setDepthWriteEnable(true)
        .setDepthCompareOp(vk::CompareOp::eLess)
        .setDepthBoundsTestEnable(false)
        .setStencilTestEnable(false);

    // 配置全局颜色混合 每个帧缓冲的单独颜色混合可以使用VkPipelineColorBlendStateCreateInfo
    vk::PipelineColorBlendAttachmentState ColorBlendAttachment = {};
    ColorBlendAttachment   //
        .setColorWriteMask(
            vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB |
            vk::ColorComponentFlagBits::eA
        )
        .setBlendEnable(true);

    vk::PipelineColorBlendStateCreateInfo ColorBlendInfo = {};
    ColorBlendInfo   //
        .setLogicOpEnable(false)
        .setLogicOp(vk::LogicOp::eCopy)   // 逻辑操作
        .setAttachments({ColorBlendAttachment});

    // 创建描述符集布局
    CreateDescriptionSetLayout();

    // 指定管线布局(uniform)
    vk::PipelineLayoutCreateInfo PipelineLayoutInfo = {};
    PipelineLayoutInfo.setSetLayouts({mDescriptorSetLayout});

    // 创建管线布局
    mPipelineLayout = Device->GetHandle().createPipelineLayout(PipelineLayoutInfo);

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
        .setSubpass(0);                             // 子Pass

    auto Pipeline = Device->GetHandle().createGraphicsPipeline(nullptr, PipelineInfo);
    if (Pipeline.result != vk::Result::eSuccess) {
        throw VulkanException(L"创建管线失败");
    } else {
        mPipeline = Pipeline.value;
    }

    // 创建多重采样颜色缓冲区
    CreateMsaaColorBuffer();
    // 创建深度缓冲区
    CreateDepthBuffer();
    // 创建交换链帧缓冲区
    CreateFramebuffers();
    LOG_INFO_CATEGORY(Vulkan, L"图形管线初始化完成");
}

SharedPtr<GraphicsPipeline> GraphicsPipeline::CreateShared(Ref<VulkanRenderer> InDevice, const GraphicsPipelineCreateInfo& InCreateInfo) {
    return MakeShared<GraphicsPipeline>(InDevice, InCreateInfo);
}

void GraphicsPipeline::Finalize() {
    if (!IsValid()) return;
    const auto& Device = mRenderer.get().GetLogicalDevice();
    const auto  Handle = Device->GetHandle();
    if (Device) {
        // 交换链缓冲区销毁
        CleanFramebuffers();
        // 深度图像销毁
        mDepthImageView->Finialize();
        mDepthImage->Finialize();
        // MSAA缓冲销毁
        mMsaaColorImageView->Finialize();
        mMsaaColorImage->Finialize();
        Handle.destroyPipeline(mPipeline);
        Handle.destroyPipelineLayout(mPipelineLayout);
        Handle.destroyDescriptorSetLayout(mDescriptorSetLayout);
        mRenderPass->Finialize();
    }
    mPipeline            = nullptr;
    mPipelineLayout      = nullptr;
    mDescriptorSetLayout = nullptr;
    LOG_INFO_CATEGORY(Vulkan, L"图形管线销毁完成");
}

void GraphicsPipeline::CreateDescriptionSetLayout() {
    Array<vk::DescriptorSetLayoutBinding> UniformBindings;
    for (const auto& UniformBinding: mShaderProg->GetUniforms() | std::views::values) {
        vk::DescriptorSetLayoutBinding Binding;
        Binding.binding         = UniformBinding.Binding;
        Binding.descriptorType  = GetVkDescriptorType(UniformBinding.Type);
        Binding.descriptorCount = 1;
        Binding.stageFlags      = GetVkShaderStage(UniformBinding.Stage);
        UniformBindings.push_back(Binding);
    }
    vk::DescriptorSetLayoutCreateInfo LayoutInfo{};
    LayoutInfo.setBindings(UniformBindings);
    mDescriptorSetLayout = mRenderer.get().GetLogicalDevice()->GetHandle().createDescriptorSetLayout(LayoutInfo);
}

void GraphicsPipeline::CreateMsaaColorBuffer() {
    const auto&     Renderer        = mRenderer;
    auto&           Device          = Renderer.get().GetLogicalDevice();
    const auto      SwapchainExtent = Renderer.get().GetSwapChainExtent();
    ImageCreateInfo ImageInfo{};
    ImageInfo.Height      = SwapchainExtent.height;
    ImageInfo.Width       = SwapchainExtent.width;
    ImageInfo.Format      = Renderer.get().GetSwapChainImageFormat();
    ImageInfo.Usage       = vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment;
    ImageInfo.SampleCount = mMsaaSamples;
    mMsaaColorImage       = Image::CreateShared(MakeRef(Device), ImageInfo);
    mMsaaColorImageView   = Device->CreateImageView(*mMsaaColorImage, ImageInfo.Format);
    mRenderer.get().GetCommandProducer()->TrainsitionImageLayout(
        mMsaaColorImage->GetHandle(), ImageInfo.Format, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal, 1
    );
}

void GraphicsPipeline::CreateDepthBuffer() {
    const auto      DepthFormat = mRenderer.get().GetDepthFormat();
    auto&           Renderer    = static_cast<VulkanRenderer&>(mRenderer);
    ImageCreateInfo ImageInfo{};
    ImageInfo.Height = Renderer.GetSwapChainExtent().height;
    ImageInfo.Width  = Renderer.GetSwapChainExtent().width;
    ImageInfo.Format = DepthFormat;
    ImageInfo.Usage  = vk::ImageUsageFlagBits::eDepthStencilAttachment;
    mDepthImage      = Image::CreateShared(MakeRef(Renderer.GetLogicalDevice()), ImageInfo);
    mDepthImageView  = Renderer.GetLogicalDevice()->CreateImageView(*mDepthImage, DepthFormat, vk::ImageAspectFlagBits::eDepth);
    Renderer.GetCommandProducer()->TrainsitionImageLayout(
        mDepthImage->GetHandle(), DepthFormat, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal, 1
    );
}

void GraphicsPipeline::CreateFramebuffers() {
    const auto& Renderer = static_cast<VulkanRenderer&>(mRenderer);
    mFramebuffers.resize(Renderer.GetSwapChainImageCount());
    for (size_t i = 0; i < mFramebuffers.size(); i++) {
        Array Attachments = {
            mMsaaColorImageView->GetHandle(),
            mDepthImageView->GetHandle(),
            Renderer.GetSwapChainImageViews()[i]->GetHandle(),
        };
        vk::FramebufferCreateInfo FramebufferInfo = {};
        FramebufferInfo   //
            .setRenderPass(mRenderPass->GetHandle())
            .setAttachments(Attachments)
            .setWidth(Renderer.GetSwapChainExtent().width)
            .setHeight(Renderer.GetSwapChainExtent().height)
            .setLayers(1);
        mFramebuffers[i] = Renderer.GetLogicalDevice()->GetHandle().createFramebuffer(FramebufferInfo);
    }
}

void GraphicsPipeline::CleanFramebuffers() {
    const auto& Device = mRenderer.get().GetLogicalDevice();
    for (const auto& Framebuffer: mFramebuffers) {
        Device->GetHandle().destroyFramebuffer(Framebuffer);
    }
    mFramebuffers.clear();
}

RHI_VULKAN_NAMESPACE_END
