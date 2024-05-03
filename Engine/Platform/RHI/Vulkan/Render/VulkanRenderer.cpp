/**
 * @file VulkanRenderer.cpp
 * @author Echo 
 * @Date 24-4-23
 * @brief 
 */

#include "VulkanRenderer.h"

#include "CoreGlobal.h"
#include "GraphicsPipeline.h"
#include "RenderPass.h"
#include "RHI/Vulkan/Instance.h"

RHI_VULKAN_NAMESPACE_BEGIN

VulkanRenderer::~VulkanRenderer() {
    if (IsValid()) {
        Finitialize();
    }
}

SharedPtr<VulkanRenderer> VulkanRenderer::CreateShared(const Instance& InVulkanInstance, UniquePtr<IRenderPassProducer> Producer) {
    auto Rtn = MakeShared<VulkanRenderer>(Protected{}, InVulkanInstance);
    Rtn->CreateGraphicsPipeline(Move(Producer));
    return Rtn;
}

VulkanRenderer::VulkanRenderer(Protected, const Instance& InVulkanInstance) {
    mRendererID = sRendererIDCount++;
    LOG_INFO_CATEGORY(Vulkan, L"创建Vulkan渲染器[id = {}]中", mRendererID, mSwapChainImageCount);
    mLogicalDevice = InVulkanInstance.CreateLogicalDeviceShared();
    mSwapChain     = mLogicalDevice->CreateSwapChain(mSwapChainImageCount, 1920, 1080);
    Initialize();
}

void VulkanRenderer::Initialize() {
    LOG_INFO_CATEGORY(Vulkan, L"Vulkan渲染器[id = {}]创建完成", mRendererID, mSwapChainImageCount);
}

void VulkanRenderer::Finitialize() {
    if (!IsValid()) return;
    mGraphicsPipeline->Finalize();
    mSwapChain->Finialize();
    mLogicalDevice->Finialize();
    LOG_INFO_CATEGORY(Vulkan, L"Vukan渲染器[id = {}]清理完成", mRendererID);
}

void VulkanRenderer::Draw() {}

bool VulkanRenderer::IsValid() const {
    return mLogicalDevice->IsValid() && mSwapChain->IsValid();
}

void VulkanRenderer::CreateGraphicsPipeline(UniquePtr<IRenderPassProducer> Producer) {
    // 创建图形管线
    // 寻找深度图像格式
    auto DepthFormat = mLogicalDevice->GetAssociatedPhysicalDevice()->FindSupportFormat(
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
        CreateInfo.RenderPassProducer = MakeUnique<DefaultRenderPassProducer>(mSwapChain->GetImageFormat(), DepthFormat);
    }
    CreateInfo.FragmentShaderPath         = L"Shaders/frag.spv";
    CreateInfo.VertexShaderPath           = L"Shaders/vert.spv";

    mGraphicsPipeline = GraphicsPipeline::CreateShared(shared_from_this(), CreateInfo);
}

RHI_VULKAN_NAMESPACE_END
