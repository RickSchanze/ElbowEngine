//
// Created by Echo on 24-12-28.
//

#pragma once
#include "Platform/RHI/RenderPass.h"

#include <vulkan/vulkan_core.h>

namespace platform::rhi::vulkan
{
class RenderPass_Vulkan final : public RenderPass
{
public:
    explicit RenderPass_Vulkan(const RenderPassCreateInfo& info);

    [[nodiscard]] void* GetNativeHandle() const override { return handle_; }

private:
    VkRenderPass handle_ = VK_NULL_HANDLE;
};
}   // namespace platform::rhi::vulkan
