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
#include "Model.h"
#include "RenderPass.h"
#include "RHI/Vulkan/VulkanContext.h"
#include "Shader.h"
#include "ShaderProgram.h"

#include <ranges>

RHI_VULKAN_NAMESPACE_BEGIN

GraphicsPipeline::GraphicsPipeline(const Ref<VulkanContext> InContext, const GraphicsPipelineCreateInfo& InCreateInfo) :
    mContext(InContext), mCreateInfo(InCreateInfo) {
    mRenderPassProducer = InCreateInfo.RenderPassProducer;
    Initialize();
    mContext.get().AddPipelineToRender(this);
}

GraphicsPipeline::~GraphicsPipeline() {
    mContext.get().RemovePipelineFromRender(this);
    Finalize();
}

SharedPtr<GraphicsPipeline> GraphicsPipeline::CreateShared(Ref<VulkanContext> InDevice, const GraphicsPipelineCreateInfo& InCreateInfo) {
    return MakeShared<GraphicsPipeline>(InDevice, InCreateInfo);
}

void GraphicsPipeline::UpdateUniformBuffer(const uint32 InCurrentImage) {
    static auto               StartTime   = std::chrono::high_resolution_clock::now();
    const auto                CurrentTime = std::chrono::high_resolution_clock::now();
    const float               Time        = std::chrono::duration<float, std::chrono::seconds::period>(CurrentTime - StartTime).count();
    StaticArray<glm::mat4, 3> UBO;
    UBO[0] = rotate(glm::mat4(1.f), Time * glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
    UBO[1] = lookAt(glm::vec3(2.f, 2.f, 2.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 1.f));
    UBO[2] = glm::perspective(
        glm::radians(45.f),
        static_cast<float>(mContext.get().GetSwapChainExtent().width) / static_cast<float>(mContext.get().GetSwapChainExtent().height),
        0.1f,
        10.f
    );
    UBO[2][1][1] *= -1;
    void* Data;
    mContext.get().GetLogicalDevice()->MapMemory(mUniformBuffersMemory[InCurrentImage], 3 * sizeof(glm::mat4), 0, &Data);
    memcpy(Data, UBO.data(), 3 * sizeof(glm::mat4));
    mContext.get().GetLogicalDevice()->UnmapMemory(mUniformBuffersMemory[InCurrentImage]);
}

vk::CommandBuffer GraphicsPipeline::GetCurrentImageCommandBuffer(const uint32 InCurrentImage) const {
    return mCommandBuffers[InCurrentImage];
}

void GraphicsPipeline::Initialize() {
    // 创建RenderPass
    if (!mCreateInfo.RenderPassProducer) {
        throw VulkanException(L"创建图形管线失败：RenderPassProducer为空");
    }
    auto&      Device             = mContext.get().GetLogicalDevice();
    const auto RenderPassProducer = mCreateInfo.RenderPassProducer->GetRenderPassCreateInfo();
    mRenderPass                   = RenderPass::CreateUnique(MakeRef(*Device), RenderPassProducer);

    const auto VertShader = Shader::CreateShared(MakeRef(*Device), mCreateInfo.VertexShaderPath, EShaderStage::Vertex);
    const auto FragShader = Shader::CreateShared(MakeRef(*Device), mCreateInfo.FragmentShaderPath, EShaderStage::Fragment);
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

    if (mCreateInfo.ViewportSize.height == 0 || mCreateInfo.ViewportSize.width == 0) {
        throw VulkanException(L"创建图形管线失败：视口大小不合法");
    }
    float Width = mContext.get().GetSwapChainExtent().width;
    float Height = mContext.get().GetSwapChainExtent().height;
    // 设置视口
    vk::Viewport ViewportInfo = {};
    ViewportInfo   //
        .setX(0)
        .setY(0)   // xy是左上角
        .setWidth(static_cast<float>(Width))
        .setHeight(static_cast<float>(Height))
        .setMinDepth(0.f)
        .setMaxDepth(1.f);   //深度的最大最小值必须在[0,1]职中

    // 裁剪矩形
    vk::Rect2D Scissor = {};
    vk::Extent2D ScissorExtent = {static_cast<uint32>(Width), static_cast<uint32>(Height)};
    Scissor   //
        .setOffset({0, 0})
        .setExtent(ScissorExtent);

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
    mMsaaSamples                                           = mCreateInfo.MsaaSamples;
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
        .setBlendEnable(false);

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
    if (mMsaaSamples != vk::SampleCountFlagBits::e1) {
        CreateMsaaColorBuffer();
    }

    // 创建深度缓冲区
    CreateDepthBuffer();
    // 创建交换链帧缓冲区
    CreateFramebuffers();

    // TODO: 重构并整个至材质系统
    CreateTextureImageAndView();
    CreateUniformBuffers();
    CreateDescriptorPool();
    CreateDescriptotSets();
    // TODO: 模型系统解耦
    LoadModel();
    CreateCommandBuffers();

    LOG_INFO_CATEGORY(Vulkan, L"图形管线初始化完成");
}

void GraphicsPipeline::Finalize() {
    if (!IsValid()) return;
    const auto& Device = mContext.get().GetLogicalDevice();
    const auto  Handle = Device->GetHandle();
    if (Device) {
        CleanDescriptorPool();
        // TODO: 模型系统解耦
        CleanModel();
        // TODO: 重构并整合至材质系统
        // 清理纹理
        CleanUniformBuffers();
        CleanTextureImageAndView();
        // 交换链缓冲区销毁
        CleanFramebuffers();
        CleanDepthBuffer();
        // MSAA缓冲销毁
        if (mMsaaSamples != vk::SampleCountFlagBits::e1) {
            mMsaaColorImageView->Finialize();
            mMsaaColorImage->Finialize();
        }
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

void GraphicsPipeline::BeginRecordCommand(const vk::CommandBuffer InBuffer) {
    vk::CommandBufferBeginInfo BeginInfo = {};
    BeginInfo.setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
    InBuffer.begin(BeginInfo);
}

void GraphicsPipeline::EndRecordCommand(const vk::CommandBuffer InBuffer) {
    InBuffer.end();
}

void GraphicsPipeline::RecordCommand(vk::CommandBuffer InBuffer, const CommandRecordingParam& InParam) {
    vk::RenderPassBeginInfo RenderPassInfo = {};
    RenderPassInfo.setRenderPass(mRenderPass->GetHandle())
        .setFramebuffer(InParam.FrameBuffer)
        .setRenderArea({{0, 0}, mContext.get().GetSwapChainExtent()});
    StaticArray<vk::ClearValue, 2> ClearValues = {};
    ClearValues[0].color                       = vk::ClearColorValue{1.f, 0.f, 0.f, 1.f};
    ClearValues[1].depthStencil                = vk::ClearDepthStencilValue{1.f, 0};
    RenderPassInfo.setClearValues(ClearValues);
    InBuffer.beginRenderPass(RenderPassInfo, vk::SubpassContents::eInline);
    InBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);
    // 使用顶点缓冲需纳入
    vk::Buffer     VertexBuffers = {mVertexBuffer};
    vk::DeviceSize Offsets       = {0};
    InBuffer.bindVertexBuffers(0, VertexBuffers, Offsets);
    InBuffer.bindIndexBuffer(mIndexBuffer, 0, vk::IndexType::eUint32);
    // 绑定描述符集
    Array Set = {InParam.DescriptorSet};
    InBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mPipelineLayout, 0, Set, {});

    InBuffer.drawIndexed(static_cast<uint32>(mModel->GetIndices().size()), 1, 0, 0, 0);

    InBuffer.endRenderPass();
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
    mDescriptorSetLayout = mContext.get().GetLogicalDevice()->GetHandle().createDescriptorSetLayout(LayoutInfo);
}

void GraphicsPipeline::CreateMsaaColorBuffer() {
    const auto&     Renderer        = mContext;
    auto&           Device          = Renderer.get().GetLogicalDevice();
    const auto      SwapchainExtent = Renderer.get().GetSwapChainExtent();
    ImageCreateInfo ImageInfo{};
    ImageInfo.Height      = SwapchainExtent.height;
    ImageInfo.Width       = SwapchainExtent.width;
    ImageInfo.Format      = Renderer.get().GetSwapChainImageFormat();
    ImageInfo.Usage       = vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment;
    ImageInfo.SampleCount = mMsaaSamples;
    mMsaaColorImage       = Image::CreateShared(*Device, ImageInfo);
    mMsaaColorImageView   = Device->CreateImageView(*mMsaaColorImage, ImageInfo.Format);
    mContext.get().GetCommandProducer()->TrainsitionImageLayout(
        mMsaaColorImage->GetHandle(), ImageInfo.Format, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal, 1
    );
}

void GraphicsPipeline::CreateDepthBuffer() {
    const auto      DepthFormat = mContext.get().GetDepthFormat();
    auto&           Renderer    = static_cast<VulkanContext&>(mContext);
    ImageCreateInfo ImageInfo{};
    ImageInfo.Height = Renderer.GetSwapChainExtent().height;
    ImageInfo.Width  = Renderer.GetSwapChainExtent().width;
    ImageInfo.Format = DepthFormat;
    ImageInfo.Usage  = vk::ImageUsageFlagBits::eDepthStencilAttachment;
    mDepthImage      = Image::CreateShared(*Renderer.GetLogicalDevice(), ImageInfo);
    mDepthImageView  = Renderer.GetLogicalDevice()->CreateImageView(*mDepthImage, DepthFormat, vk::ImageAspectFlagBits::eDepth);
    Renderer.GetCommandProducer()->TrainsitionImageLayout(
        mDepthImage->GetHandle(), DepthFormat, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal, 1
    );
}

void GraphicsPipeline::CleanDepthBuffer() {
    mDepthImageView->Finialize();
    mDepthImage->Finialize();
}

void GraphicsPipeline::CreateFramebuffers() {
    auto& Context = static_cast<VulkanContext&>(mContext);
    mFramebuffers.resize(Context.GetSwapChainImageCount());
    for (size_t i = 0; i < mFramebuffers.size(); i++) {
        Array<vk::ImageView> Attachments;
        if (mMsaaSamples == vk::SampleCountFlagBits::e1) {
            Attachments = {
                Context.GetSwapChainImageViews()[i]->GetHandle(),
                mDepthImageView->GetHandle(),
            };
        } else {
            Attachments = {
                mMsaaColorImageView->GetHandle(),
                mDepthImageView->GetHandle(),
                Context.GetSwapChainImageViews()[i]->GetHandle(),
            };
        }
        vk::FramebufferCreateInfo FramebufferInfo = {};
        FramebufferInfo   //
            .setRenderPass(mRenderPass->GetHandle())
            .setAttachments(Attachments)
            .setWidth(Context.GetSwapChainExtent().width)
            .setHeight(Context.GetSwapChainExtent().height)
            .setLayers(1);
        mFramebuffers[i] = Context.GetLogicalDevice()->GetHandle().createFramebuffer(FramebufferInfo);
    }
}

void GraphicsPipeline::CleanFramebuffers() {
    const auto& Device = mContext.get().GetLogicalDevice();
    for (const auto& Framebuffer: mFramebuffers) {
        Device->GetHandle().destroyFramebuffer(Framebuffer);
    }
    mFramebuffers.clear();
}

void GraphicsPipeline::LoadModel() {
    mModel                                = MakeShared<Resource::Model>(L"Models/viking_room.obj");
    // 顶点缓冲
    const vk::DeviceSize VertexBufferSize = sizeof(mModel->GetVertices()[0]) * mModel->GetVertices().size();
    vk::Buffer           VertexStagingBuffer;
    vk::DeviceMemory     VertexStagingBufferMemory;
    mContext.get().GetLogicalDevice()->CreateBuffer(
        VertexBufferSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        VertexStagingBuffer,
        VertexStagingBufferMemory
    );
    void* Data;
    mContext.get().GetLogicalDevice()->MapMemory(VertexStagingBufferMemory, VertexBufferSize, 0, &Data);
    memcpy(Data, mModel->GetVertices().data(), static_cast<size_t>(VertexBufferSize));
    mContext.get().GetLogicalDevice()->UnmapMemory(VertexStagingBufferMemory);
    mContext.get().GetLogicalDevice()->CreateBuffer(
        VertexBufferSize,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        mVertexBuffer,
        mVertexBufferMemory
    );
    mContext.get().GetCommandProducer()->CopyBuffer(VertexStagingBuffer, mVertexBuffer, VertexBufferSize);
    mContext.get().GetLogicalDevice()->GetHandle().destroyBuffer(VertexStagingBuffer);
    mContext.get().GetLogicalDevice()->GetHandle().freeMemory(VertexStagingBufferMemory);

    // 索引缓冲
    const vk::DeviceSize IndexBufferSize = sizeof(mModel->GetIndices()[0]) * mModel->GetIndices().size();
    vk::Buffer           IndexStagingBuffer;
    vk::DeviceMemory     IndexStagingBufferMemory;
    mContext.get().GetLogicalDevice()->CreateBuffer(
        IndexBufferSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        IndexStagingBuffer,
        IndexStagingBufferMemory
    );
    mContext.get().GetLogicalDevice()->MapMemory(IndexStagingBufferMemory, IndexBufferSize, 0, &Data);
    memcpy(Data, mModel->GetIndices().data(), static_cast<size_t>(IndexBufferSize));
    mContext.get().GetLogicalDevice()->UnmapMemory(IndexStagingBufferMemory);
    mContext.get().GetLogicalDevice()->CreateBuffer(
        IndexBufferSize,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        mIndexBuffer,
        mIndexBufferMemory
    );
    mContext.get().GetCommandProducer()->CopyBuffer(IndexStagingBuffer, mIndexBuffer, IndexBufferSize);
    mContext.get().GetLogicalDevice()->GetHandle().destroyBuffer(IndexStagingBuffer);
    mContext.get().GetLogicalDevice()->GetHandle().freeMemory(IndexStagingBufferMemory);
}

void GraphicsPipeline::CleanModel() {
    mContext.get().GetLogicalDevice()->GetHandle().destroyBuffer(mVertexBuffer);
    mContext.get().GetLogicalDevice()->GetHandle().freeMemory(mVertexBufferMemory);
    mContext.get().GetLogicalDevice()->GetHandle().destroyBuffer(mIndexBuffer);
    mContext.get().GetLogicalDevice()->GetHandle().freeMemory(mIndexBufferMemory);
}

void GraphicsPipeline::CreateTextureImageAndView() {
    const auto Texture = Resource::Texture::Create(L"Textures/viking_room.png");
    mTexture           = Texture::Create(*mContext.get().GetLogicalDevice(), *mContext.get().GetCommandProducer(), Texture);
    mTextureView       = mContext.get().GetLogicalDevice()->CreateImageView(
        *mTexture, vk::Format::eR8G8B8A8Unorm, vk::ImageAspectFlagBits::eColor, mTexture->GetMipLevel()
    );
    CreateTextureSampler();
}

void GraphicsPipeline::CleanTextureImageAndView() {
    mTextureView->Finialize();
    mTexture->Finialize();
    CleanTextureSampler();
}

void GraphicsPipeline::CreateTextureSampler() {
    vk::SamplerCreateInfo SamplerInfo   = {};
    SamplerInfo.magFilter               = vk::Filter::eLinear;
    SamplerInfo.minFilter               = vk::Filter::eLinear;
    SamplerInfo.addressModeU            = vk::SamplerAddressMode::eRepeat;
    SamplerInfo.addressModeV            = vk::SamplerAddressMode::eRepeat;
    SamplerInfo.addressModeW            = vk::SamplerAddressMode::eRepeat;
    SamplerInfo.anisotropyEnable        = true;
    SamplerInfo.maxAnisotropy           = 16;
    SamplerInfo.borderColor             = vk::BorderColor::eIntOpaqueBlack;
    // false则纹理坐标为(0,1)
    SamplerInfo.unnormalizedCoordinates = false;
    // 与一个特定值比较，通常阴影贴图会用到
    SamplerInfo.compareEnable           = false;
    SamplerInfo.compareOp               = vk::CompareOp::eAlways;
    // Mipmap
    SamplerInfo.mipmapMode              = vk::SamplerMipmapMode::eLinear;
    SamplerInfo.mipLodBias              = 0;
    SamplerInfo.minLod                  = 0;
    SamplerInfo.maxLod                  = 0;
    mTextureSampler                     = mContext.get().GetLogicalDevice()->GetHandle().createSampler(SamplerInfo);
}

void GraphicsPipeline::CleanTextureSampler() {
    mContext.get().GetLogicalDevice()->GetHandle().destroy(mTextureSampler);
}

void GraphicsPipeline::CreateUniformBuffers() {
    vk::DeviceSize BufferSize = 0;
    for (const auto& [Key, Value]: mShaderProg->GetUniforms()) {
        BufferSize += Value.Size;
    }
    mUniformBuffers.resize(mContext.get().GetSwapChainImageCount());
    mUniformBuffersMemory.resize(mContext.get().GetSwapChainImageCount());
    for (size_t i = 0; i < mContext.get().GetSwapChainImageCount(); i++) {
        mContext.get().GetLogicalDevice()->CreateBuffer(
            BufferSize,
            vk::BufferUsageFlagBits::eUniformBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            mUniformBuffers[i],
            mUniformBuffersMemory[i]
        );
    }
}
void GraphicsPipeline::CleanUniformBuffers() {
    for (size_t i = 0; i < mContext.get().GetSwapChainImageCount(); i++) {
        mContext.get().GetLogicalDevice()->GetHandle().destroyBuffer(mUniformBuffers[i]);
        mContext.get().GetLogicalDevice()->GetHandle().freeMemory(mUniformBuffersMemory[i]);
    }
    mUniformBuffers.clear();
    mUniformBuffersMemory.clear();
}

void GraphicsPipeline::CreateDescriptorPool() {
    StaticArray<vk::DescriptorPoolSize, 2> PoolSizes = {};
    // Uniform Object
    PoolSizes[0].type                                = vk::DescriptorType::eUniformBuffer;
    PoolSizes[0].descriptorCount                     = mContext.get().GetSwapChainImageCount();

    // 纹理采样器
    PoolSizes[1].type            = vk::DescriptorType::eCombinedImageSampler;
    PoolSizes[1].descriptorCount = mContext.get().GetSwapChainImageCount();

    vk::DescriptorPoolCreateInfo PoolInfo = {};
    PoolInfo.setPoolSizes(PoolSizes).setMaxSets(mContext.get().GetSwapChainImageCount());
    mDescriptorPool = mContext.get().GetLogicalDevice()->GetHandle().createDescriptorPool(PoolInfo);
}

void GraphicsPipeline::CleanDescriptorPool() {
    mContext.get().GetLogicalDevice()->GetHandle().destroyDescriptorPool(mDescriptorPool);
}

void GraphicsPipeline::CreateDescriptotSets() {
    Array<vk::DescriptorSetLayout> Layouts(mContext.get().GetSwapChainImageCount(), mDescriptorSetLayout);
    vk::DescriptorSetAllocateInfo  AllocInfo = {};
    AllocInfo.setDescriptorPool(mDescriptorPool).setSetLayouts(Layouts);
    mDescriptorSets.resize(mContext.get().GetSwapChainImageCount());
    // 描述符池对象销毁时会自动清除描述符集
    mDescriptorSets = mContext.get().GetLogicalDevice()->GetHandle().allocateDescriptorSets(AllocInfo);
    for (size_t i = 0; i < mDescriptorSets.size(); i++) {
        vk::DescriptorBufferInfo BufferInfo = {};
        BufferInfo.setBuffer(mUniformBuffers[i]).setOffset(0).setRange(VK_WHOLE_SIZE);
        vk::DescriptorImageInfo ImageInfo = {};
        ImageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
            .setImageView(mTextureView->GetHandle())
            .setSampler(mTextureSampler);

        StaticArray<vk::WriteDescriptorSet, 2> DescriptorWrites = {};

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
        mContext.get().GetLogicalDevice()->GetHandle().updateDescriptorSets(DescriptorWrites, nullptr);
    }
}

void GraphicsPipeline::CreateCommandBuffers() {
    vk::CommandBufferAllocateInfo AllocInfo = {};
    AllocInfo.setCommandPool(mContext.get().GetCommandProducer()->GetCommandPool())
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandBufferCount(mContext.get().GetSwapChainImageCount());

    mCommandBuffers = mContext.get().GetCommandProducer()->CreateCommandBuffers(AllocInfo);

    for (size_t i = 0; i < mCommandBuffers.size(); i++) {
        auto CommandBuffer = mCommandBuffers[i];
        BeginRecordCommand(CommandBuffer);
        {
            CommandRecordingParam Param;
            Param.FrameBuffer   = mFramebuffers[i];
            Param.DescriptorSet = mDescriptorSets[i];
            RecordCommand(CommandBuffer, Param);
        }
        EndRecordCommand(CommandBuffer);
    }
}

void GraphicsPipeline::CleanCommandBuffers() {
    mContext.get().GetCommandProducer()->DestroyCommandBuffers(mCommandBuffers);
}

void GraphicsPipeline::SubmitGraphicsQueue(
    int CurrentImageIndex, vk::Queue InGraphicsQueue, Array<vk::Semaphore> InWaitSemaphores, Array<vk::Semaphore> InSingalSemaphores,
    vk::Fence InFrameFence
) {
    auto                   CmdBuffer  = GetCurrentImageCommandBuffer(CurrentImageIndex);
    vk::SubmitInfo SubmitInfo = {};
    vk::PipelineStageFlags WaitFlag = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    SubmitInfo.setCommandBuffers(CmdBuffer)
        .setWaitSemaphores(InWaitSemaphores)
        .setSignalSemaphores(InSingalSemaphores)
        .setWaitDstStageMask(WaitFlag);
    InGraphicsQueue.submit(SubmitInfo, InFrameFence);
}

void GraphicsPipeline::Rebuild() {
    Finalize();
    Initialize();
}


RHI_VULKAN_NAMESPACE_END
