/**
 * @file VulkanContext.cpp
 * @author Echo 
 * @Date 24-4-23
 * @brief 
 */

#include "VulkanContext.h"

#include "../../../Tool/EngineApplication.h"
#include "CoreGlobal.h"
#include "Instance.h"
#include "Render/CommandPool.h"
#include "Render/GraphicsPipeline.h"
#include "Render/RenderPass.h"
#include "Utils/ContainerUtils.h"
#include "Utils/StringUtils.h"

#include <iostream>

RHI_VULKAN_NAMESPACE_BEGIN

VulkanContext::~VulkanContext() {
    if (IsValid()) {
        Finalize();
    }
}

TUniquePtr<VulkanContext> VulkanContext::CreateUnique(const TSharedPtr<Instance>& InVulkanInstance) {
    auto Rtn = MakeUnique<VulkanContext>(Protected{}, InVulkanInstance);
    Rtn->CreateGraphicsPipeline();
    return Rtn;
}

VulkanContext::VulkanContext(Protected, const TSharedPtr<Instance>& InVulkanInstance) {
    if (sContext == nullptr) {
        sContext = this;
    } else {
        LOG_INFO_CATEGORY(Vulkan, L"多次初始化VulkanContext,忽略本次初始化调用");
        return;
    }
    mRendererID = sRendererIDCount++;
    LOG_INFO_CATEGORY(Vulkan, L"创建Vulkan渲染器[id = {}]中", mRendererID, mSwapChainImageCount);
    mVulkanInstance       = InVulkanInstance;
    mPhysicalDevice       = mVulkanInstance->PickPhysicalDevice();
    const auto Properties = mPhysicalDevice->GetProperties();
    auto       Name       = StringUtils::FromAnsiString(Properties.deviceName);
    LOG_INFO_CATEGORY(Vulkan, L"物理设备选择完成. 选用: {}", Name);
    mLogicalDevice = mPhysicalDevice->CreateLogicalDeviceUnique();
    mSwapChain     = mLogicalDevice->CreateSwapChain(mSwapChainImageCount, 1920, 1080);
    // 初始化命令生产者
    mCommandPool   = CommandPool::CreateUnique(mLogicalDevice);
    CreateSyncObjecs();
    Initialize();
}
VulkanContext& VulkanContext::Get() {
    if (sContext == nullptr) {
        throw VulkanException(L"VulkanContext未初始化");
    }
    return *sContext;
}

void VulkanContext::Initialize() {
    LOG_INFO_CATEGORY(Vulkan, L"Vulkan渲染器[id = {}]创建完成", mRendererID, mSwapChainImageCount);
}

void VulkanContext::Finalize() {
    if (!IsValid()) return;
    CleanSyncObjects();
    mCommandPool->Finialize();
    // 当前GraphicsPipeline创建时自己加载文件因此有可能抛出异常，此时mGraphicsPipeline为nullptr
    // 在调用就成了未定义行为，因此加一个if判断
    // TODO: 将Shader文件读取操作放在GraphicsPipeline之外
    if (mGraphicsPipeline) {
        delete mGraphicsPipeline;
        mGraphicsPipeline = nullptr;
    }
    mSwapChain->Finialize();
    mLogicalDevice->Finialize();
    LOG_INFO_CATEGORY(Vulkan, L"Vukan渲染器[id = {}]清理完成", mRendererID);
}

void VulkanContext::Draw() {
    const vk::Device                 Device = mLogicalDevice->GetHandle();
    // 首先等待当前帧的指令缓冲结束执行
    const TStaticArray<vk::Fence, 1> Fence  = {mInFlightFences[mCurrentFrame]};
    auto                             _      = Device.waitForFences(Fence, VK_TRUE, UINT64_MAX);
    // 获取可用图像索引
    uint32                           ImageIndex;
    VkResult                         AcquireResult =
        vkAcquireNextImageKHR(Device, mSwapChain->GetHandle(), UINT64_MAX, mImageAvailableSemaphores[mCurrentFrame], nullptr, &ImageIndex);
    if (AcquireResult == VK_ERROR_OUT_OF_DATE_KHR) {
        // 交换链已经过期，需要重建
        RebuildSwapChain();
        return;
    } else if (AcquireResult != VK_SUCCESS && AcquireResult != VK_SUBOPTIMAL_KHR) {
        throw VulkanException(L"无法获取交换链图像");
    }

    Device.resetFences(Fence);
    // 这里更新UniformBuffer
    mGraphicsPipeline->UpdateUniformBuffer(ImageIndex);

    TArray                WaitSemaphores   = {mImageAvailableSemaphores[mCurrentFrame]};
    TArray<vk::Semaphore> SingalSemaphores = {};

    constexpr vk::SemaphoreCreateInfo SemaphoreInfo = {};
    for (auto& Pipeline: mRenderGraphicsPipelines) {
        SingalSemaphores.push_back(Device.createSemaphore(SemaphoreInfo));
    }

    for (int i = 0; i < mRenderGraphicsPipelines.size(); i++) {
        mRenderGraphicsPipelines[i]->SubmitGraphicsQueue(
            ImageIndex, mLogicalDevice->GetGraphicsQueue(), WaitSemaphores, {SingalSemaphores[i]}, mInFlightFences[mCurrentFrame]
        );
    }

    // 呈现
    vk::PresentInfoKHR PresentInfo = {};
    TStaticArray       SwapChains  = {mSwapChain->GetHandle()};
    PresentInfo.setWaitSemaphores(SingalSemaphores)
        .setSwapchains(SwapChains)
        .setImageIndices(ImageIndex);

    const auto Result = mLogicalDevice->GetPresentQueue().presentKHR(&PresentInfo);

    if (Result == vk::Result::eErrorOutOfDateKHR || Result == vk::Result::eSuboptimalKHR ||
        Tool::EngineApplication::Get().bFrameBufferResized) {
        RebuildSwapChain();
        Tool::EngineApplication::Get().bFrameBufferResized = false;
    } else if (Result != vk::Result::eSuccess) {
        throw VulkanException(std::format(L"呈现交换链图像失败: {}", StringUtils::FromAnsiString(to_string(Result))));
    }

    mLogicalDevice->GetPresentQueue().waitIdle();
    for (auto& Semaphore: SingalSemaphores) {
        Device.destroySemaphore(Semaphore);
    }
    mCurrentFrame = (mCurrentFrame + 1) % mMaxFramesInFlight;
}

bool VulkanContext::IsValid() const {
    // clang-format off
    return
        mSwapChain && mSwapChain->IsValid() &&
        mCommandPool &&
        mLogicalDevice && mLogicalDevice->IsValid() &&
        mPhysicalDevice && mPhysicalDevice->IsValid() &&
        mVulkanInstance && mVulkanInstance->IsValid();
    // clang-format on
}

void VulkanContext::RebuildSwapChain() {
    // TODO: 使用PipelineCache
    auto Size = Tool::EngineApplication::Get().GetWindowSize();
    while (Size.Width == 0 || Size.Height == 0) {
        Size = Tool::EngineApplication::Get().GetWindowSize();
        glfwWaitEvents();
    }
    mLogicalDevice->GetGraphicsQueue().waitIdle();
    mLogicalDevice->GetPresentQueue().waitIdle();
    mLogicalDevice->GetHandle().waitIdle();

    mSwapChain->Finialize();
    mSwapChain = mLogicalDevice->CreateSwapChain(mSwapChainImageCount, Size.Width, Size.Height);

    for (const auto& Pipeline: mRenderGraphicsPipelines) {
        Pipeline->Rebuild();
    }
}

vk::Format VulkanContext::GetDepthImageFormat() {
    return mPhysicalDevice->FindSupportFormat(
        {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
}

void VulkanContext::CreateGraphicsPipeline() {
    // 创建图形管线
    // 寻找深度图像格式
    PipelineInitializer Initializer;
    Initializer.ShaderStage.FragmentShaderPath = L"Shaders/frag.spv";
    Initializer.ShaderStage.VertexShaderPath   = L"Shaders/vert.spv";
    mGraphicsPipeline                          = new GraphicsPipeline(Initializer);
}

// void VulkanContext::AddPipelineToRender(IGraphicsPipeline* InPipeline) {
//     mRenderGraphicsPipelines.push_back(InPipeline);
// }
//
// void VulkanContext::RemovePipelineFromRender(IGraphicsPipeline* InPipeline) {
//     ContainerUtils::Remove(mRenderGraphicsPipelines, InPipeline);
// }

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
