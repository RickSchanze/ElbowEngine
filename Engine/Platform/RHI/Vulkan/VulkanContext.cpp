/**
 * @file VulkanContext.cpp
 * @author Echo 
 * @Date 24-4-23
 * @brief 
 */

#include "VulkanContext.h"

#include "CoreGlobal.h"
#include "Instance.h"
#include "Render/CommandProducer.h"
#include "Render/GraphicsPipeline.h"
#include "Render/RenderPass.h"
#include "Utils/StringUtils.h"

#include <iostream>

RHI_VULKAN_NAMESPACE_BEGIN

VulkanContext::~VulkanContext() {
    if (IsValid()) {
        Finalize();
    }
}

UniquePtr<VulkanContext> VulkanContext::CreateUnique(const SharedPtr<Instance>& InVulkanInstance, UniquePtr<IRenderPassProducer> Producer) {
    auto Rtn = MakeUnique<VulkanContext>(Protected{}, InVulkanInstance);
    Rtn->CreateGraphicsPipeline(Move(Producer), *Rtn);
    return Rtn;
}

VulkanContext::VulkanContext(Protected, const SharedPtr<Instance>& InVulkanInstance) {
    mRendererID = sRendererIDCount++;
    LOG_INFO_CATEGORY(Vulkan, L"创建Vulkan渲染器[id = {}]中", mRendererID, mSwapChainImageCount);
    mVulkanInstance       = InVulkanInstance;
    mPhysicalDevice       = mVulkanInstance->PickPhysicalDevice();
    const auto Properties = mPhysicalDevice->GetProperties();
    auto       Name       = StringUtils::FromAnsiString(Properties.deviceName);
    LOG_INFO_CATEGORY(Vulkan, L"物理设备选择完成. 选用: {}", Name);
    mLogicalDevice   = mPhysicalDevice->CreateLogicalDeviceUnique();
    mSwapChain       = mLogicalDevice->CreateSwapChain(mSwapChainImageCount, 1920, 1080);
    // 初始化命令生产者
    mCommandProducer = CommandProducer::CreateUnique(mLogicalDevice);
    CreateSyncObjecs();
    Initialize();
}

void VulkanContext::Initialize() {
    LOG_INFO_CATEGORY(Vulkan, L"Vulkan渲染器[id = {}]创建完成", mRendererID, mSwapChainImageCount);
}

void VulkanContext::Finalize() {
    if (!IsValid()) return;
    CleanSyncObjects();
    mCommandProducer->Finialize();
    // 当前GraphicsPipeline创建时自己加载文件因此有可能抛出异常，此时mGraphicsPipeline为nullptr
    // 在调用就成了未定义行为，因此加一个if判断
    // TODO: 将Shader文件读取操作放在GraphicsPipeline之外
    if (mGraphicsPipeline) mGraphicsPipeline->Finalize();
    mSwapChain->Finialize();
    mLogicalDevice->Finialize();
    LOG_INFO_CATEGORY(Vulkan, L"Vukan渲染器[id = {}]清理完成", mRendererID);
}

void VulkanContext::Draw() {
    vk::Device        Device = mLogicalDevice->GetHandle();
    // 首先等待当前帧的指令缓冲结束执行
    const StaticArray Fence  = {mInFlightFences[mCurrentFrame]};
    Device.waitForFences(Fence, VK_TRUE, UINT64_MAX);
    // 获取可用图像索引
    const auto ImageIndex =
        Device.acquireNextImageKHR(mSwapChain->GetHandle(), UINT64_MAX, mImageAvailableSemaphores[mCurrentFrame], nullptr);
    if (ImageIndex.result == vk::Result::eErrorOutOfDateKHR) {
        // 交换链已经过期，需要重建
        RebuildSwapChain();
        return;
    } else if (ImageIndex.result != vk::Result::eSuccess && ImageIndex.result != vk::Result::eSuboptimalKHR) {
        throw VulkanException(L"无法获取交换链图像");
    }

    Device.resetFences(Fence);
    // 这里更新UniformBuffer
    mGraphicsPipeline->UpdateUniformBuffer(ImageIndex.value);

    // 提交渲染指令
    vk::SubmitInfo SubmitInfo = {};

    StaticArray WaitStages       = {static_cast<vk::PipelineStageFlags>(vk::PipelineStageFlagBits::eColorAttachmentOutput)};
    StaticArray WaitSemaphores   = {mImageAvailableSemaphores[mCurrentFrame]};
    StaticArray CurrentBuffer    = {mGraphicsPipeline->GetCurrentImageCommandBuffer(ImageIndex.value)};
    StaticArray SingalSemaphores = {mImageRenderFinishedSemaphores[mCurrentFrame]};

    SubmitInfo.setWaitSemaphores(WaitSemaphores)
        .setWaitDstStageMask(WaitStages)
        .setCommandBuffers(CurrentBuffer)
        .setSignalSemaphores(SingalSemaphores);

    mLogicalDevice->GetGraphicsQueue().submit({SubmitInfo}, mInFlightFences[mCurrentFrame]);

    // 呈现
    vk::PresentInfoKHR PresentInfo = {};
    StaticArray        SwapChains  = {mSwapChain->GetHandle()};
    PresentInfo.setWaitSemaphores(SingalSemaphores).setSwapchains(SwapChains).setImageIndices(ImageIndex.value);
    const auto Result = mLogicalDevice->GetPresentQueue().presentKHR(PresentInfo);
    if (Result == vk::Result::eErrorOutOfDateKHR || Result == vk::Result::eSuboptimalKHR) {
        RebuildSwapChain();
    } else if (Result != vk::Result::eSuccess) {
        throw VulkanException(std::format(L"呈现交换链图像失败: {}", StringUtils::FromAnsiString(to_string(Result))));
    }
    mLogicalDevice->GetPresentQueue().waitIdle();
    mCurrentFrame = (mCurrentFrame + 1) % mMaxFramesInFlight;
}

bool VulkanContext::IsValid() const {
    // clang-format off
    return
        mSwapChain && mSwapChain->IsValid() &&
        mCommandProducer &&
        mLogicalDevice && mLogicalDevice->IsValid() &&
        mPhysicalDevice && mPhysicalDevice->IsValid() &&
        mVulkanInstance && mVulkanInstance->IsValid();
    // clang-format on
}

void VulkanContext::RebuildSwapChain() {}

void VulkanContext::CreateGraphicsPipeline(UniquePtr<IRenderPassProducer> Producer, const Ref<VulkanContext> InRenderer) {
    // 创建图形管线
    // 寻找深度图像格式
    mDepthFormat = mPhysicalDevice->FindSupportFormat(
        {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
    GraphicsPipelineCreateInfo CreateInfo = {};
    CreateInfo.ViewportSize               = mSwapChain->GetExtent();
    CreateInfo.MsaaSamples                = vk::SampleCountFlagBits::e1;
    if (Producer != nullptr) {
        CreateInfo.RenderPassProducer = Move(Producer);
    } else {
        CreateInfo.RenderPassProducer = MakeUnique<DefaultRenderPassProducer>(mSwapChain->GetImageFormat(), mDepthFormat);
    }
    CreateInfo.FragmentShaderPath = L"Shaders/frag.spv";
    CreateInfo.VertexShaderPath   = L"Shaders/vert.spv";

    mGraphicsPipeline = GraphicsPipeline::CreateShared(InRenderer, CreateInfo);
}

void VulkanContext::CreateInstance() {}

void VulkanContext::CreateSyncObjecs() {
    mImageAvailableSemaphores.resize(mMaxFramesInFlight);
    mImageRenderFinishedSemaphores.resize(mMaxFramesInFlight);
    mInFlightFences.resize(mMaxFramesInFlight);

    vk::SemaphoreCreateInfo SemaphoreInfo = {};
    vk::FenceCreateInfo     FenceInfo     = {};
    FenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

    // 为多帧并行渲染创建信号量
    for (size_t i = 0; i < mMaxFramesInFlight; i++) {
        mImageAvailableSemaphores[i]      = mLogicalDevice->GetHandle().createSemaphore(SemaphoreInfo);
        mImageRenderFinishedSemaphores[i] = mLogicalDevice->GetHandle().createSemaphore(SemaphoreInfo);
        mInFlightFences[i]                = mLogicalDevice->GetHandle().createFence(FenceInfo);
    }
}

void VulkanContext::CleanSyncObjects() {
    for (size_t i = 0; i < mMaxFramesInFlight; i++) {
        mLogicalDevice->GetHandle().destroySemaphore(mImageAvailableSemaphores[i]);
        mLogicalDevice->GetHandle().destroySemaphore(mImageRenderFinishedSemaphores[i]);
        mLogicalDevice->GetHandle().destroyFence(mInFlightFences[i]);
    }
}

RHI_VULKAN_NAMESPACE_END
