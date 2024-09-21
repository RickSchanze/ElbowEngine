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
    TOptional<uint32_t> graphics_family;
    TOptional<uint32_t> present_family;

    [[nodiscard]] bool IsValid() const { return graphics_family.has_value() && present_family.has_value(); }
};

class PhysicalDevice
{
public:
    static inline TArray<const char*> s_device_required_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#ifdef ENABLE_PROFILING
        VK_KHR_CALIBRATED_TIMESTAMPS_EXTENSION_NAME,
#endif
    };
    static TArray<const char*> GetDeviceRequiredExtensions() { return s_device_required_extensions; }

    struct SwapChainSupportDetails
    {
        vk::SurfaceCapabilitiesKHR   capabilities;
        TArray<vk::SurfaceFormatKHR> formats;
        TArray<vk::PresentModeKHR>   present_modes;
    };

public:
    typedef PhysicalDevice ThisClass;

    explicit PhysicalDevice(Instance* InAttachedInstance) : instance_(InAttachedInstance) {}

    explicit operator vk::PhysicalDevice() const { return handle_; }

    explicit operator bool() const { return IsValid(); }

    bool IsValid() const { return static_cast<bool>(handle_) && instance_ != nullptr; }

    /**
     * 使用Instance选择合适的物理设备
     * @param instance 此物理设备附着的Instance
     * @param pick_func 用来确定一个设备是否合适
     * @return
     */
    static TUniquePtr<PhysicalDevice>
    PickPhysicalDevice(Instance* instance, const TFunction<bool(const PhysicalDevice&)>& pick_func = &ThisClass::IsDeviceSuitable);

    /**
     * 判断一个Device是否合适
     * @param device
     * @return
     */
    static bool IsDeviceSuitable(const PhysicalDevice& device);

    PhysicalDevice& SetVulkanPhysicalDevice(vk::PhysicalDevice device);

    /**
     * 寻找此物理设备支持的FamilyIndices
     * @return 支持的FamilyIndices
     */
    QueueFamilyIndices FindQueueFamilyIndices() const;

    /**
     * 看此物理是被是否支持所有需要的扩展
     * @param required_extensions
     * @return
     */
    bool CheckExtensionSupport(const TArray<const char*>& required_extensions) const;

    /**
     * 查询此物理设备对交换链的支持情况
     * @return
     */
    [[nodiscard]] SwapChainSupportDetails QuerySwapChainSupport() const;

    /**
     * 在InCandidates中找到一个支持InTiling和InFeatures的格式
     * @param candidates
     * @param tiling
     * @param features
     * @return
     */
    vk::Format FindSupportFormat(const TArray<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlagBits features) const;

    uint32_t FindMemoryType(uint32_t type_filter, vk::MemoryPropertyFlags properties) const;


    TUniquePtr<LogicalDevice> CreateLogicalDeviceUnique();
    vk::Device                CreateLogicalDeviceHandle() const;

    [[nodiscard]] Instance* GetAttachedInstance() const { return instance_; }

    // 一些转发函数
    // clang-format off
    [[nodiscard]] vk::PhysicalDevice GetHandle() const { return handle_; }
    [[nodiscard]] vk::PhysicalDeviceProperties GetProperties() const { return handle_.getProperties(); }
    [[nodiscard]] vk::PhysicalDeviceFeatures GetFeatures() const { return handle_.getFeatures(); }
    // clang-format on

private:
    // 实际的vkPhysicalDevice
    vk::PhysicalDevice            handle_;
    // 附着的VkInstance实例
    Instance*                     instance_ = nullptr;
    // 支持的FamilyIndices
    QueueFamilyIndices            supported_queue_family_indices_;
};

RHI_VULKAN_NAMESPACE_END
