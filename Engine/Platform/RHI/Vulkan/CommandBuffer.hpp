//
// Created by Echo on 2025/3/22.
//
#pragma once
#include "Platform/RHI/CommandBuffer.hpp"
#include "vulkan/vulkan.h"

namespace RHI {
    class CommandPool_Vulkan final : public CommandPool {
    public:
        explicit CommandPool_Vulkan(CommandPoolCreateInfo info);

        virtual ~CommandPool_Vulkan() override;

        virtual void* GetNativeHandle() const override
        {
            return command_pool_;
        }

        virtual SharedPtr<CommandBuffer> CreateCommandBuffer(bool self_managed = false) override;

        virtual Array<SharedPtr<CommandBuffer>> CreateCommandBuffers(uint32_t count, bool self_managed) override;

        void FreeCommandBuffer(VkCommandBuffer buffer);

        virtual void Reset() override;

    private:
        VkCommandPool command_pool_;
    };

    /**
     * 这个由CommandPool管理其内存 因此不提供析构函数
     */
    class CommandBuffer_Vulkan : public CommandBuffer {
    public:
        explicit CommandBuffer_Vulkan(VkCommandBuffer buffer, CommandPool_Vulkan* pool, bool self_managed = false)
            : buffer_(buffer), pool_(pool), self_managed_(self_managed)
        {
        }

        virtual ~CommandBuffer_Vulkan() override;

        virtual void Reset() override { vkResetCommandBuffer(buffer_, 0); }

        virtual void* GetNativeHandle() const override
        {
            return buffer_;
        }

        virtual exec::ExecFuture<> Execute() override;

        virtual void Begin() override;

        virtual void End() override;

    private:
        VkCommandBuffer buffer_ = VK_NULL_HANDLE;
        CommandPool_Vulkan *pool_ = nullptr;
        bool self_managed_ = false;
    };
} // namespace rhi
