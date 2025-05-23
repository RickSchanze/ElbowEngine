//
// Created by Echo on 2025/3/22.
//
#pragma once
#include "vulkan/vulkan.h"
#include "Platform/RHI/RenderPass.hpp"

namespace NRHI {
    class RenderPass_Vulkan final : public RenderPass {
    public:
        explicit RenderPass_Vulkan(const RenderPassCreateInfo &info);

        virtual void *GetNativeHandle() const override { return handle_; }

    private:
        VkRenderPass handle_ = VK_NULL_HANDLE;
    };
} // namespace rhi
