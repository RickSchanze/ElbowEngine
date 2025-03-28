//
// Created by Echo on 2025/3/22.
//
#pragma once
#include "Core/Misc/SharedPtr.hpp"
#include "Platform/RHI/Enums.hpp"
#include "Platform/RHI/Image.hpp"
#include "vulkan/vulkan.h"

namespace rhi {
    class Image_Vulkan final : public Image {
        friend class GfxContext_Vulkan;

    public:
        explicit Image_Vulkan(const ImageDesc &desc);

        /// **仅** 供交换链图像使用
        explicit Image_Vulkan(VkImage handle_, Int32 index, UInt32 width_, UInt32 height_, Format format_);

        ~Image_Vulkan() override;

        [[nodiscard]] void *GetNativeHandle() const override { return image_handle_; }

        [[nodiscard]] VkDeviceMemory GetNativeMemory() const { return image_memory_; }

        SharedPtr<Buffer> CreateCPUVisibleBuffer() override;

    protected:
        VkImage image_handle_ = VK_NULL_HANDLE;
        VkDeviceMemory image_memory_ = VK_NULL_HANDLE;
    };

    class Sampler_Vulkan final : public Sampler {
    public:
        explicit Sampler_Vulkan(const SamplerDesc &desc);

        ~Sampler_Vulkan() override;

        [[nodiscard]] void *GetNativeHandle() const override { return sampler_handle_; }

    private:
        VkSampler sampler_handle_ = VK_NULL_HANDLE;
    };


} // namespace rhi
