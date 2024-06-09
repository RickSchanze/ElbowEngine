/**
 * @file PhysicalDevice.h
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "Render/LogicalDevice.h"
#include "VulkanCommon.h"

#include "vulkan/vulkan.hpp"

RHI_VULKAN_NAMESPACE_BEGIN
class Instance;

struct QueueFamilyIndices
{
    TOptional<UInt32> GraphicsFamily;
    TOptional<UInt32> PresentFamily;

    [[nodiscard]] bool IsValid() const { return GraphicsFamily.has_value() && PresentFamily.has_value(); }
};

class PhysicalDevice {
public:
    static inline TArray<const char*> sDeviceRequiredExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        // VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
    };
    static TArray<const char*> GetDeviceRequiredExtensions() { return sDeviceRequiredExtensions; }

    struct SwapChainSupportDetails
    {
        vk::SurfaceCapabilitiesKHR  Capabilities;
        TArray<vk::SurfaceFormatKHR> Formats;
        TArray<vk::PresentModeKHR>   PresentModes;
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
    static TUniquePtr<PhysicalDevice>
    PickPhysicalDevice(Instance* InAttachedInstance, const TFunction<bool(const PhysicalDevice&)>& PickFunc = &ThisClass::IsDeviceSuitable);

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
    [[nodiscard]] bool CheckExtensionSupport(const TArray<const char*>& RequiredExtensions) const;

    /**
     * 查询此物理设备对交换链的支持情况
     * @return
     */
    [[nodiscard]] SwapChainSupportDetails QuerySwapChainSupport() const;

    /**
     * 在InCandidates中找到一个支持InTiling和InFeatures的格式
     * @param InCandidates
     * @param InTiling
     * @param InFeatures
     * @return
     */
    vk::Format
    FindSupportFormat(const TArray<vk::Format>& InCandidates, vk::ImageTiling InTiling, vk::FormatFeatureFlagBits InFeatures) const;

    UInt32 FindMemoryType(UInt32 InTypeFilter, vk::MemoryPropertyFlags InProperties) const;


    TUniquePtr<LogicalDevice> CreateLogicalDeviceUnique();
    vk::Device               CreateLogicalDeviceHandle() const;

    [[nodiscard]] Instance* GetAttachedInstance() const { return mAttachedInstance; }

    // 一些转发函数
    // clang-format off
    [[nodiscard]] vk::PhysicalDevice GetHandle() const { return mDeviceHandle; }
    [[nodiscard]] vk::PhysicalDeviceProperties GetProperties() const { return mDeviceHandle.getProperties(); }
    [[nodiscard]] vk::PhysicalDeviceFeatures GetFeatures() const { return mDeviceHandle.getFeatures(); }
    // clang-format on

private:
    // 实际的vkPhysicalDevice
    vk::PhysicalDevice            mDeviceHandle;
    // 附着的VkInstance实例
    Instance*                     mAttachedInstance = nullptr;
    // 支持的FamilyIndices
    QueueFamilyIndices            mSupportedQueueFamilyIndices;
};

RHI_VULKAN_NAMESPACE_END
