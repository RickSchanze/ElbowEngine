//
// Created by Echo on 2025/3/22.
//
#pragma once
#include "Platform/RHI/LowShader.hpp"
#include "vulkan/vulkan.h"

namespace rhi {
    class LowShader_Vulkan : public LowShader {
    public:
        LowShader_Vulkan(const char *code, size_t size);

        ~LowShader_Vulkan() override;

        [[nodiscard]] void *GetNativeHandle() const override { return handle_; }

    protected:
        VkShaderModule handle_ = VK_NULL_HANDLE;
    };
} // namespace rhi
