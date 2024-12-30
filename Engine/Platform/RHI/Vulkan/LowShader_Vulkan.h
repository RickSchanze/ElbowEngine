//
// Created by Echo on 24-12-26.
//

#pragma once
#include "Platform/RHI/LowShader.h"
#include "vulkan/vulkan.h"

namespace platform::rhi::vulkan
{
class LowShader_Vulkan : public LowShader
{
public:
    LowShader_Vulkan(const char* code, size_t size);

    ~LowShader_Vulkan() override;

    [[nodiscard]] void* GetNativeHandle() const override { return handle_; }

protected:
    VkShaderModule handle_ = VK_NULL_HANDLE;
};
}   // namespace platform::rhi::vulkan
