/**
 * @file LogicalDevice.h
 * @author Echo 
 * @Date 24-4-21
 * @brief 
 */

#pragma once
#include "Image.h"
#include "ImageView.h"
#include "RHI/Vulkan/Interface/IRHIResource.h"
#include "RHI/Vulkan/VulkanCommon.h"
#include "vulkan/vulkan.hpp"

namespace RHI::Vulkan {
class SwapChain;
}
namespace RHI::Vulkan {
class PhysicalDevice;
}
namespace RHI::Vulkan {
class Instance;
}
RHI_VULKAN_NAMESPACE_BEGIN

class LogicalDevice final : public IRHIResource {
public:
     LogicalDevice() = default;
    ~LogicalDevice() override;

    static SharedPtr<LogicalDevice> CreateShared(vk::Device InDevice, const WeakPtr<PhysicalDevice>& InAssociatedPhysicalDevice);
    static UniquePtr<LogicalDevice> CreateUnique(vk::Device InDevice, const WeakPtr<PhysicalDevice>& InAssociatedPhysicalDevice);

    explicit LogicalDevice(ResourcePrivate, vk::Device InDevice, const WeakPtr<PhysicalDevice>& InAssociatedPhysicalDevice);

    void Initialize() override;
    void Finialize() override;

    /**
     * 创建一个交换链对象
     * @param InSwapChainImageCount 交换链图像数量 为0表示自动选择
     * @param InWidth
     * @param InHeight
     * @return
     */
    UniquePtr<SwapChain> CreateSwapChain(uint32 InSwapChainImageCount = 0, uint32 InWidth = 0, uint32 InHeight = 0);

    /**
     * 基于InImage创建一个图像视图
     * @param InImage
     * @param InFormat
     * @param InAspectFlags
     * @param InMipLevels
     * @return
     */
    SharedPtr<ImageView> CreateImageView(
        const ImageBase& InImage, vk::Format InFormat, vk::ImageAspectFlags InAspectFlags = vk::ImageAspectFlagBits::eColor,
        uint32 InMipLevels = 1
    );

    bool       IsValid() const override {
        return static_cast<bool>(mLogicalDeviceHandle) && !mAssociatedPhysicalDevice.expired();
    }
    vk::Device GetHandle() const { return mLogicalDeviceHandle; }
    SharedPtr<PhysicalDevice> GetAssociatedPhysicalDevice() const { return mAssociatedPhysicalDevice.lock(); }
    vk::Queue                 GetGraphicsQueue() const { return mGraphicsQueue; }
    vk::Queue                 GetPresentQueue() const { return mPresentQueue; }

private:
    vk::Device              mLogicalDeviceHandle = VK_NULL_HANDLE;
    WeakPtr<PhysicalDevice> mAssociatedPhysicalDevice;

    vk::Queue mGraphicsQueue;
    vk::Queue mPresentQueue;
};

RHI_VULKAN_NAMESPACE_END
