/**
 * @file LogicalDevice.cpp
 * @author Echo 
 * @Date 24-4-21
 * @brief 
 */

#include "LogicalDevice.h"

#include "CoreGlobal.h"
#include "RHI/Vulkan/Instance.h"
#include "RHI/Vulkan/PhysicalDevice.h"
#include "SwapChain.h"

RHI_VULKAN_NAMESPACE_BEGIN

LogicalDevice::~LogicalDevice() {
    if (!IsValid()) return;
    Finalize();
}

SharedPtr<LogicalDevice> LogicalDevice::CreateShared(vk::Device InDevice, const WeakPtr<PhysicalDevice>& InAssociatedPhysicalDevice) {
    return MakeShared<LogicalDevice>(ResourcePrivate{}, InDevice, InAssociatedPhysicalDevice);
}

UniquePtr<LogicalDevice> LogicalDevice::CreateUnique(vk::Device InDevice, const WeakPtr<PhysicalDevice>& InAssociatedPhysicalDevice) {
    return MakeUnique<LogicalDevice>(ResourcePrivate{}, InDevice, InAssociatedPhysicalDevice);
}

LogicalDevice::LogicalDevice(ResourcePrivate, const vk::Device InDevice, const WeakPtr<PhysicalDevice>& InAssociatedPhysicalDevice) :
    mLogicalDeviceHandle(InDevice), mAssociatedPhysicalDevice(InAssociatedPhysicalDevice) {
    Initialize();
}

void LogicalDevice::Initialize() {}

void LogicalDevice::Finalize() {
    mLogicalDeviceHandle.destroy();
    mLogicalDeviceHandle = VK_NULL_HANDLE;
}

UniquePtr<SwapChain> LogicalDevice::CreateSwapChain(const uint32 InSwapChainImageCount) {
    if (mAssociatedPhysicalDevice.expired())
        throw VulkanException(L"LogicalDevice::CreateSwapChain: 无法创建交换链: mAssociatedPhysicalDevice失效");
    const auto AssociatedPhysicalDevice = mAssociatedPhysicalDevice.lock();

    const auto SwapChainSupport = AssociatedPhysicalDevice->QuerySwapChainSupport();
    const auto Surface          = AssociatedPhysicalDevice->GetAttachedInstance()->GetSurfaceHandle();

    const auto SurfaceFormat = SwapChain::ChooseSwapSurfaceFormat(SwapChainSupport.Formats);
    const auto PresentMode   = SwapChain::ChooseSwapPresentMode(SwapChainSupport.PresentModes);
    const auto Extent        = SwapChain::ChooseSwapExtent(SwapChainSupport.Capabilities);

    uint32 ImageCount = InSwapChainImageCount;
    if (InSwapChainImageCount == 0) {
        ImageCount = SwapChainSupport.Capabilities.minImageCount + 1;
    }
    if (SwapChainSupport.Capabilities.maxImageCount > 0 && ImageCount > SwapChainSupport.Capabilities.maxImageCount) {
        ImageCount = SwapChainSupport.Capabilities.maxImageCount;
    }
    vk::SwapchainCreateInfoKHR SwapChainInfo = {};
    // clang-format off
    SwapChainInfo
        .setSurface(Surface)                                             // 设置表面
        .setMinImageCount(ImageCount)                                    // 设置交换链图像数量
        .setImageFormat(SurfaceFormat.format)                            // 设置图像格式
        .setImageColorSpace(SurfaceFormat.colorSpace)                    // 设置颜色空间
        .setImageExtent(Extent)                                          // 设置图像大小(分辨率)
        .setImageArrayLayers(1)                                          // 每个图像包含的层次 通常是1 对于VR应用可能更多
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)         // 将图像作为颜色附着
        .setPreTransform(SwapChainSupport.Capabilities.currentTransform) // 指定一个固定的图像变换操作（需要交换链具有supportedTransforms特性）例如旋转90度
        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)       // 指定alhpa通道是否和窗口系统中其他窗口混合，通常使用这个值忽略alpha通道
        .setPresentMode(PresentMode)                                     // 设置呈现模式
        .setClipped(true)                                                // 不关心窗口系统中其他窗口遮挡的颜色
        .setOldSwapchain(VK_NULL_HANDLE);                                // 如果交换链需要重新创建，可以指定一个旧的交换链
    // clang-format on

    // 指定在多个队列族中使用交换链图像的方式
    const auto                   Indicies            = AssociatedPhysicalDevice->FindQueueFamilyIndices();
    const StaticArray<uint32, 2> QueueFamilyIndicies = {
        Indicies.GraphicsFamily.value(),
        Indicies.PresentFamily.value(),
    };
    if (Indicies.GraphicsFamily != Indicies.PresentFamily) {
        SwapChainInfo
            .setImageSharingMode(vk::SharingMode::eConcurrent)   // 图像可以在多个队列族使用而不需要显式改变图像所有权
            .setQueueFamilyIndices(QueueFamilyIndicies);         // 不是同一队列族时需要指定此项
    } else {
        SwapChainInfo.setImageSharingMode(vk::SharingMode::eExclusive); // 图像同一时间只能被一个队列族用于，此时无需指定FamilyIndices
    }
    return SwapChain::CreateUnique(mLogicalDeviceHandle.createSwapchainKHR(SwapChainInfo), this);
}

RHI_VULKAN_NAMESPACE_END
