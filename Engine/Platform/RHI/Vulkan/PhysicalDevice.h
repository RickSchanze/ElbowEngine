/**
 * @file PhysicalDevice.h
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "VulkanCommon.h"

#include "vulkan/vulkan.hpp"

RHI_VULKAN_NAMESPACE_BEGIN
class Instance;

struct QueueFamilyIndices
{
    Optional<uint32> GraphicsFamily;
    Optional<uint32> PresentFamily;

    [[nodiscard]] bool IsValid() const {
        return GraphicsFamily.has_value() && PresentFamily.has_value();
    }
};

class PhysicalDevice {
public:
    static inline Set<AnsiString> sDeviceRequiredExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    struct SwapChainSupportDetails
    {
        vk::SurfaceCapabilitiesKHR Capabilities;
        Array<vk::SurfaceFormatKHR> Formats;
        Array<vk::PresentModeKHR> PresentModes;
    };

public:
    typedef PhysicalDevice ThisClass;
    explicit PhysicalDevice(Instance* InAttachedInstance) : mAttachedInstance(InAttachedInstance) {}
    explicit operator vk::PhysicalDevice() const { return mDeviceHandle; }
    explicit operator bool() const { return IsValid(); }
    [[nodiscard]] bool IsValid() const { return static_cast<bool>(mDeviceHandle); }

    /**
     * 使用Instance选择合适的物理设备
     * @param InAttachedInstance 此物理设备附着的Instance
     * @param PickFunc 用来确定一个设备是否合适
     * @return
     */
    static UniquePtr<PhysicalDevice> PickPhysicalDevice(
        Instance* InAttachedInstance,
        const Function<bool(const PhysicalDevice&)>& PickFunc = &ThisClass::IsDeviceSuitable
    );

    /**
     * 判断一个Device是否合适
     * @param InDevice
     * @return
     */
    static bool IsDeviceSuitable(const PhysicalDevice& InDevice);

    PhysicalDevice& SetVulkanPhysicalDevice(vk::PhysicalDevice InDevice);

    /**
     * 寻找此物理设备支持的FamilyIndices
     * @return 支持的FamilyIndices
     */
    [[nodiscard]] QueueFamilyIndices FindQueueFamilyIndices() const;

    /**
     * 看此物理是被是否支持所有需要的扩展
     * @param RequiredExtensions
     * @return
     */
    [[nodiscard]] bool CheckExtensionSupport(const Set<AnsiString>& RequiredExtensions) const;

    /**
     * 查询此物理设备对交换链的支持情况
     * @return
     */
    [[nodiscard]] SwapChainSupportDetails QuerySwapChainSupport();

private:
    // 实际的vkPhysicalDevice
    vk::PhysicalDevice mDeviceHandle;
    // 附着的VkInstance实例
    Instance* mAttachedInstance = nullptr;
    // 支持的FamilyIndices
    QueueFamilyIndices mSupportedQueueFamilyIndices;
};

RHI_VULKAN_NAMESPACE_END
