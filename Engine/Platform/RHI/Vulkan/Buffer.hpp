//
// Created by Echo on 2025/3/22.
//
#pragma once
#include "Platform/RHI/Buffer.hpp"
#include "vulkan/vulkan.h"

namespace RHI {
    class Buffer_Vulkan final : public Buffer {
    public:
        explicit Buffer_Vulkan(const BufferDesc& info);

        virtual ~Buffer_Vulkan() override;

        virtual void* GetNativeHandle() const override
        {
            return buffer_;
        }

        virtual UInt8* BeginWrite() override;
        virtual void EndWrite() override;

        virtual UInt8* BeginRead() override;

        virtual void EndRead() override;

    private:
        VkBuffer buffer_ = VK_NULL_HANDLE;
        VkDeviceMemory memory_ = VK_NULL_HANDLE;
    };
} // namespace rhi
