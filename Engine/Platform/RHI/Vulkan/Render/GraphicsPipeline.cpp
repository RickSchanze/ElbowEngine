/**
 * @file GraphicsPipeline.cpp
 * @author Echo 
 * @Date 24-4-27
 * @brief 
 */

#include "GraphicsPipeline.h"

#include "CoreGlobal.h"
#include "LogicalDevice.h"
#include "RenderPass.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "VulkanRenderer.h"

#include <ranges>

RHI_VULKAN_NAMESPACE_BEGIN


// TODO: 将管线加入程序启动流程
GraphicsPipeline::GraphicsPipeline(VulkanRenderer* InRenderer, const GraphicsPipelineCreateInfo& InCreateInfo) {
    // 创建RenderPass
    if (!InCreateInfo.RenderPassProducer) {
        throw VulkanException(L"创建图形管线失败：RenderPassProducer为空");
    }
    mRenderer                     = InRenderer;
    const auto Device             = InRenderer->GetLogicalDevice();
    const auto RenderPassProducer = InCreateInfo.RenderPassProducer->GetRenderPassCreateInfo();
    const auto mRenderPass        = RenderPass::CreateUnique(RenderPassProducer, Device);

    const auto VertShader = Shader::CreateShared(Device, InCreateInfo.VertexShaderPath, EShaderStage::Vertex);
    const auto FragShader = Shader::CreateShared(Device, InCreateInfo.FragmentShaderPath, EShaderStage::Fragment);
    mShaderProg           = ShaderProgram::CreateShared(Device, VertShader, FragShader);

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
    LOG_INFO_CATEGORY(Vulkan, L"图形管线初始化完成");
}

SharedPtr<GraphicsPipeline> GraphicsPipeline::CreateShared(VulkanRenderer* InDevice, const GraphicsPipelineCreateInfo& InCreateInfo) {
    return MakeShared<GraphicsPipeline>(InDevice, InCreateInfo);
}

void GraphicsPipeline::Finalize() const {
    const auto Device = mRenderer->GetLogicalDevice();
    const auto Handle = Device->GetHandle();
    if (Device) {
        Handle.destroyPipeline(mPipeline);
        Handle.destroyPipelineLayout(mPipelineLayout);
        Handle.destroyDescriptorSetLayout(mDescriptorSetLayout);
        mRenderPass->Finialize();
    }
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
    mDescriptorSetLayout = mRenderer->GetLogicalDevice()->GetHandle().createDescriptorSetLayout(LayoutInfo);
}

void GraphicsPipeline::CreateMsaaColorBuffer() {
    if (mMsaaSamples == vk::SampleCountFlagBits::e1) {
        // 不需要多重采样
        return;
    }
    const auto      Renderer        = mRenderer;
    const auto      Device          = Renderer->GetLogicalDevice();
    const auto      SwapchainExtent = Renderer->GetSwapChainExtent();
    ImageCreateInfo ImageInfo{};
    ImageInfo.Height      = SwapchainExtent.height;
    ImageInfo.Width       = SwapchainExtent.width;
    ImageInfo.Format = Renderer->GetSwapChainImageFormat();
    ImageInfo.Usage = vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment;
    ImageInfo.SampleCount = mMsaaSamples;
    mMsaaColorImage = Image::CreateShared(Device, ImageInfo);
    mMsaaColorImageView = Device->CreateImageView(*mMsaaColorImage, ImageInfo.Format);
}

RHI_VULKAN_NAMESPACE_END
