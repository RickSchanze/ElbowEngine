/**
 * @file LogicalDevice.h
 * @author Echo 
 * @Date 24-4-21
 * @brief 
 */

#pragma once
#include "Interface/IRHIResource.h"
#include "vulkan/vulkan.hpp"
#include "VulkanCommon.h"

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

    explicit LogicalDevice(const vk::Device InDevice, PhysicalDevice* InAssociatedPhysicalDevice) :
        mLogicalDeviceHandle(InDevice), mAssociatedPhysicalDevice(InAssociatedPhysicalDevice) {}
    void Initialize() override;
    void Finalize() override;

    /**
     * 创建一个交换链对象
     * @param InSwapChainImageCount 交换链图像数量 为0表示自动选择
     * @return
     */
    SwapChain CreateSwapChain(uint32 InSwapChainImageCount = 0);

    [[nodiscard]] bool IsValid() const override { return static_cast<bool>(mLogicalDeviceHandle) && mAssociatedPhysicalDevice != nullptr; }

    [[nodiscard]] vk::Device      GetLogicalDeviceHandle() const { return mLogicalDeviceHandle; }
    [[nodiscard]] PhysicalDevice& GetAssociatedPhysicalDevice() const { return *mAssociatedPhysicalDevice; }

private:
    vk::Device      mLogicalDeviceHandle      = VK_NULL_HANDLE;
    PhysicalDevice* mAssociatedPhysicalDevice = nullptr;
};

RHI_VULKAN_NAMESPACE_END
