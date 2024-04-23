/**
 * @file VulkanRenderer.h
 * @author Echo 
 * @Date 24-4-23
 * @brief 
 */

#pragma once
#include "LogicalDevice.h"
#include "RHI/Vulkan/Instance.h"
#include "RHI/Vulkan/VulkanCommon.h"
#include "SwapChain.h"

RHI_VULKAN_NAMESPACE_BEGIN

class VulkanRenderer {
public:
    VulkanRenderer() = default;

    ~VulkanRenderer();

    static SharedPtr<VulkanRenderer> CreateShared(const Instance& InVulkanInstance);

    // 请不要直接调用此函数，请使用VulkanRenderer::Create
    explicit VulkanRenderer(const Instance& InVulkanInstance);

public:
    void Initialize();
    void Finitialize();

    void Draw();

    [[nodiscard]] bool IsValid() const;

private:
    SharedPtr<LogicalDevice> mLogicalDevice;

    UniquePtr<SwapChain> mSwapChain;
    int32                mSwapChainImageCount = 3;

    int32 mRendererID = 0;

    static inline int32 sRendererIDCount = 0;
};

RHI_VULKAN_NAMESPACE_END
