//
// Created by Echo on 2025/3/22.
//

#pragma once
#include "Core/Async/Exec/ExecFuture.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "Core/String/String.hpp"
#include "Core/Memory/FrameAllocator.hpp"
#include "GfxContext.hpp"
#include "IResource.hpp"


namespace rhi {
    struct RHICommand;
    enum class QueueFamilyType;
    struct CommandPoolCreateInfo {
        QueueFamilyType type;
        bool allow_reset; // 允许分配的command buffer在提交后可以被reset

        CommandPoolCreateInfo(QueueFamilyType type, const bool allow_reset) : type(type), allow_reset(allow_reset) {}
    };

    /**
     * CommandBuffer只能从CommandPool创建
     */
    class CommandBuffer : public IResource {
    public:
        ~CommandBuffer() override = default;

        /**
         * 交给渲染线程去翻译
         * 执行完成后会调用Clear
         * @param label label 这一段命令的标签(for debug)
         */
        virtual exec::ExecFuture<> Execute(StringView label) = 0;

        /**
         * 清理队里里的命令 之前记录过的不会被清理
         */
        void Clear() { commands_.Clear(); }

        void EnqueueCommand(RHICommand *command) { commands_.Add(command); }

        template<typename T, typename... Args>
        void Enqueue(Args &&...args) {
            RHICommand *cmd = NewFrameTemp<T>(GetGfxContextRef().GetCommandAllocator(), Forward<Args>(args)...);
            EnqueueCommand(cmd);
        }

        virtual void Reset() = 0;

        virtual void Begin() = 0;
        virtual void End() = 0;

    protected:
        Array<RHICommand *> commands_;
    };

    class CommandPool : public IResource {
    public:
        ~CommandPool() override = default;

        explicit CommandPool(CommandPoolCreateInfo info) {}

        /**
         * 创建一堆CommandBuffer
         * @param count
         * @param self_managed 为true则其析构函数会调用Free, 否则由CommandPoolReset时释放
         * @return
         */
        virtual Array<SharedPtr<CommandBuffer>> CreateCommandBuffers(UInt32 count, bool self_managed) = 0;

        /**
         * 创建一个CommandBuffer
         * @param self_managed 为true则其析构函数会调用Free, 否则由CommandPoolReset时释放
         * @return
         */
        virtual SharedPtr<CommandBuffer> CreateCommandBuffer(bool self_managed) = 0;

        /**
         * 重置CommandPool, 同时会重置所有的CommandBuffer
         */
        virtual void Reset() = 0;
    };
} // namespace rhi
