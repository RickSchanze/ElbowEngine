/**
 * @file PhysicalDevice.h
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "LogicalDevice.h"
#include "VulkanCommon.h"

#include "vulkan/vulkan.hpp"

RHI_VULKAN_NAMESPACE_BEGIN
class Instance;

struct QueueFamilyIndices
{
    Optional<uint32> GraphicsFamily;
    Optional<uint32> PresentFamily;

    [[nodiscard]] bool IsValid() const { return GraphicsFamily.has_value() && PresentFamily.has_value(); }
};

class PhysicalDevice {
public:
    static inline Array<const char*> sDeviceRequiredExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        // VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
    };
    static inline Array<const char*> GetDeviceRequiredExtensions() { return sDeviceRequiredExtensions; }

    struct SwapChainSupportDetails
    {
        vk::SurfaceCapabilitiesKHR  Capabilities;
        Array<vk::SurfaceFormatKHR> Formats;
        Array<vk::PresentModeKHR>   PresentModes;
    };

public:
    typedef PhysicalDevice ThisClass;
    explicit               PhysicalDevice(Instance* InAttachedInstance) : mAttachedInstance(InAttachedInstance) {}
    explicit               operator vk::PhysicalDevice() const { return mDeviceHandle; }
    explicit               operator bool() const { return IsValid(); }
    [[nodiscard]] bool     IsValid() const { return static_cast<bool>(mDeviceHandle) && mAttachedInstance != nullptr; }

    /**
     * 使用Instance选择合适的物理设备
     * @param InAttachedInstance 此物理设备附着的Instance
     * @param PickFunc 用来确定一个设备是否合适
     * @return
     */
    static UniquePtr<PhysicalDevice>
    PickPhysicalDevice(Instance* InAttachedInstance, const Function<bool(const PhysicalDevice&)>& PickFunc = &ThisClass::IsDeviceSuitable);

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
    [[nodiscard]] bool CheckExtensionSupport(const Array<const char*>& RequiredExtensions) const;

    /**
     * 查询此物理设备对交换链的支持情况
     * @return
     */
    [[nodiscard]] SwapChainSupportDetails QuerySwapChainSupport() const;

    /**
     * 以此物理设备为基础创建一个逻辑设备
     * @return
     */
    LogicalDevice CreateLogicalDevice();

    /**
     * 获得以此物理设备为基础创建的逻辑设备
     * @return
     */
    [[nodiscard]] LogicalDevice GetAssociatedLogicalDevice() const { return mAssociatedLogicalDevice; }

    [[nodiscard]] Instance* GetAttachedInstance() const { return mAttachedInstance; }

    // 一些转发函数
    // clang-format off
    [[nodiscard]] vk::PhysicalDevice GetVulkanPhysicalDeviceHandle() const { return mDeviceHandle; }
    [[nodiscard]] vk::PhysicalDeviceProperties GetProperties() const { return mDeviceHandle.getProperties(); }
    [[nodiscard]] vk::PhysicalDeviceFeatures GetFeatures() const { return mDeviceHandle.getFeatures(); }
    // clang-format on

private:
    // 实际的vkPhysicalDevice
    vk::PhysicalDevice mDeviceHandle;
    // 附着的VkInstance实例
    Instance*          mAttachedInstance = nullptr;
    // 支持的FamilyIndices
    QueueFamilyIndices mSupportedQueueFamilyIndices;
    // 由此物理设备创建的逻辑设备
    LogicalDevice      mAssociatedLogicalDevice;
};

RHI_VULKAN_NAMESPACE_END
