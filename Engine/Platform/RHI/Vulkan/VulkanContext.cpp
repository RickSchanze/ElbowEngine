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
    Initialize();
}

void VulkanContext::Initialize() {
    LOG_INFO_CATEGORY(Vulkan, L"Vulkan渲染器[id = {}]创建完成", mRendererID, mSwapChainImageCount);
}

void VulkanContext::Finalize() {
    if (!IsValid()) return;
    mCommandProducer->Finialize();
    // 当前GraphicsPipeline创建时自己加载文件因此有可能抛出异常，此时mGraphicsPipeline为nullptr
    // 在调用就成了未定义行为，因此加一个if判断
    // TODO: 将Shader文件读取操作放在GraphicsPipeline之外
    if (mGraphicsPipeline) mGraphicsPipeline->Finalize();
    mSwapChain->Finialize();
    mLogicalDevice->Finialize();
    LOG_INFO_CATEGORY(Vulkan, L"Vukan渲染器[id = {}]清理完成", mRendererID);
}

void VulkanContext::Draw() {}

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

void VulkanContext::CreateInstance(){

}

RHI_VULKAN_NAMESPACE_END
