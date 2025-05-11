//
// Created by Echo on 2025/3/22.
//
#pragma once
#include "Core/Misc/SharedPtr.hpp"
#include "Platform/RHI/Enums.hpp"
#include "Platform/RHI/Image.hpp"
#include "vulkan/vulkan.h"

namespace NRHI {
    class CommandBuffer;
}
namespace NRHI {
    class Image_Vulkan final : public Image {
        friend class GfxContext_Vulkan;

    public:
        explicit Image_Vulkan(const ImageDesc &desc);

        /// **仅** 供交换链图像使用
        explicit Image_Vulkan(VkImage handle_, Int32 index, UInt32 width_, UInt32 height_, Format format_);

        virtual ~Image_Vulkan() override;

        virtual void *GetNativeHandle() const override { return image_handle_; }

        VkDeviceMemory GetNativeMemory() const
        {
            return image_memory_;
        }

        virtual SharedPtr<Buffer> CreateCPUVisibleBuffer() override;

        UInt8 GetFormatComponentSize();

        // TODO: 从文件直接加载mipmap链
        void GenerateMipmaps(CommandBuffer& cmd);

    protected:
        VkImage image_handle_ = VK_NULL_HANDLE;
        VkDeviceMemory image_memory_ = VK_NULL_HANDLE;
    };

    class Sampler_Vulkan final : public Sampler {
    public:
        explicit Sampler_Vulkan(const SamplerDesc& desc);

        virtual ~Sampler_Vulkan() override;

        virtual void *GetNativeHandle() const override { return sampler_handle_; }

    private:
        VkSampler sampler_handle_ = VK_NULL_HANDLE;
    };


} // namespace rhi
