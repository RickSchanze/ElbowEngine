//
// Created by Echo on 2025/3/22.
//

#pragma once
#include "Commands.hpp"
#include "Core/Async/Exec/ExecFuture.hpp"
#include "Core/Memory/FrameAllocator.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "Core/String/String.hpp"
#include "GfxContext.hpp"
#include "IResource.hpp"
#include "Image.hpp"


namespace rhi {
    class ComputePipeline;
}
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
         */
        virtual exec::ExecFuture<> Execute() = 0;

        /// 利用ImagePipeline更改图形布局(2025.4.3理解)
        void ImagePipelineBarrier(ImageLayout old_layout, ImageLayout new_layout, Image *target, const ImageSubresourceRange &subresource_range,
                                  AccessFlags src_access, AccessFlags dst_access, PipelineStageFlags src_stage, PipelineStageFlags dst_stage);
        void ImagePipelineBarrier(ImageLayout old_layout, ImageLayout new_layout, const SharedPtr<Image> &target,
                                  const ImageSubresourceRange &subresource_range, AccessFlags src_access, AccessFlags dst_access,
                                  PipelineStageFlags src_stage, PipelineStageFlags dst_stage);

        /// 插入一段Commands Debug标签
        void BeginDebugLabel(StringView label);
        /// 结束Debug标签
        void EndDebugLabel();
        /// 开始渲染
        void BeginRender(const Array<RenderAttachment> &colors, const RenderAttachment &depth = {}, Vector2f size = {0, 0});
        /// 结束渲染
        void EndRender();
        /// 设置Scissor
        void SetScissor(const Rect2Df &scissor);
        /// 设置Viewport
        void SetViewport(const Rect2Df &viewport);
        /// 设置当前Pipeline
        void BindPipeline(Pipeline *pipeline);
        void BindComputePipeline(Pipeline *pipeline);
        /// 绑定DescriptorSet
        void BindDescriptorSet(Pipeline *pipeline, DescriptorSet *set);
        void BindDescriptorSetCompute(Pipeline *pipeline, DescriptorSet *set);
        /// 绑定VertexBuffer
        void BindVertexBuffer(Buffer *buffer, UInt32 offset = 0, UInt32 binding = 0);
        void BindVertexBuffer(const SharedPtr<Buffer> &buffer, UInt32 offset = 0, UInt32 binding = 0);
        /// 绑定IndexBuffer
        void BindIndexBuffer(Buffer *buffer_, UInt64 offset_ = 0);
        void BindIndexBuffer(const SharedPtr<Buffer> &buffer, UInt64 offset_ = 0);
        /// 绘制
        void DrawIndexed(uint32_t index_count_, uint32_t instance_count_ = 1, UInt32 first_index_ = 0 DEBUG_ONLY(, const String &debug_name = ""));
        /// 传输指令
        void CopyBufferToImage(Buffer *src, Image *dst, const ImageSubresourceRange &subresource_range, Vector3i offset, Vector3i size);
        void CopyImageToBuffer(Image *src, Buffer *dst, const ImageSubresourceRange &subresource_range, Vector3i offset, Vector3i size);
        void CopyBuffer(Buffer *src, Buffer *dst, UInt64 size = 0);
        void Dispatch(UInt32 x, UInt32 y, UInt32 z) { Enqueue<Cmd_Dispatch>(x, y, z); }

        /**
         * 清理队里里的命令 之前记录过的不会被清理
         */
        void Clear() { commands_.Clear(); }

        void EnqueueCommand(RHICommand *command) { commands_.Add(command); }

        void BlitImage(Image* src, Image* dst, const ImageBlitDesc& desc);

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
