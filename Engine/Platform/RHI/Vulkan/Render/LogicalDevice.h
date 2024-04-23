/**
 * @file LogicalDevice.h
 * @author Echo 
 * @Date 24-4-21
 * @brief 
 */

#pragma once
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
    void Finalize() override;

    /**
     * 创建一个交换链对象
     * @param InSwapChainImageCount 交换链图像数量 为0表示自动选择
     * @return
     */
    UniquePtr<SwapChain> CreateSwapChain(uint32 InSwapChainImageCount = 0);

    [[nodiscard]] bool IsValid() const override { return static_cast<bool>(mLogicalDeviceHandle) && !mAssociatedPhysicalDevice.expired(); }

    [[nodiscard]] vk::Device                GetLogicalDeviceHandle() const { return mLogicalDeviceHandle; }
    [[nodiscard]] SharedPtr<PhysicalDevice> GetAssociatedPhysicalDevice() const { return mAssociatedPhysicalDevice.lock(); }

private:
    vk::Device              mLogicalDeviceHandle = VK_NULL_HANDLE;
    WeakPtr<PhysicalDevice> mAssociatedPhysicalDevice;
};

RHI_VULKAN_NAMESPACE_END
