//
// Created by Echo on 2025/3/22.
//
#pragma once
#include "Platform/RHI/Buffer.hpp"
#include "vulkan/vulkan.h"

namespace rhi {
    class Buffer_Vulkan final : public Buffer {
    public:
        explicit Buffer_Vulkan(const BufferDesc &info);

        ~Buffer_Vulkan() override;

        [[nodiscard]] void *GetNativeHandle() const override { return buffer_; }

        UInt8 *BeginWrite() override;
        void EndWrite() override;

        UInt8 *BeginRead() override;

        void EndRead() override;

    private:
        VkBuffer buffer_ = VK_NULL_HANDLE;
        VkDeviceMemory memory_ = VK_NULL_HANDLE;
    };
} // namespace rhi
