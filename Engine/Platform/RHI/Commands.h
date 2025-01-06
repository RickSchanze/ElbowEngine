//
// Created by Echo on 24-12-21.
//

#pragma once
#include "Core/CoreGlobal.h"
#include "Core/Math/MathTypes.h"

namespace platform::rhi
{
class Buffer;
}
namespace platform::rhi
{

enum class RHICommandType
{
    CopyBuffer,
    BindVertexBuffer,
    BindIndexBuffer,
    DrawIndexed,
    BindGraphicsPipeline,
    BeginRender,
    EndRender,
    BeginRenderPass,
    EndRenderPass,
    Count,
};

struct RHICommand
{
    virtual ~RHICommand()                                = default;
    [[nodiscard]] virtual RHICommandType GetType() const = 0;
};

struct Cmd_CopyBuffer final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override { return RHICommandType::CopyBuffer; }

    Buffer* src;
    Buffer* dst;
    size_t  size = 0;

    Cmd_CopyBuffer(Buffer* src_, Buffer* dst_, const size_t size_ = 0) : src(src_), dst(dst_), size(size_) {}
};

struct Cmd_BindVertexBuffer final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override { return RHICommandType::BindVertexBuffer; }

    explicit Cmd_BindVertexBuffer(Buffer* buffer_, UInt64 offset_ = 0) : buffer(buffer_), offset(offset_) {}

    explicit Cmd_BindVertexBuffer(const core::SharedPtr<Buffer>& buffer_, UInt64 offset_ = 0) : buffer(buffer_.get()), offset(offset_) {}

    Buffer* buffer;
    UInt64  offset = 0;
};

struct Cmd_BindIndexBuffer final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override { return RHICommandType::BindIndexBuffer; }

    explicit Cmd_BindIndexBuffer(Buffer* buffer_, UInt64 offset_ = 0) : buffer(buffer_), offset(offset_) {}
    explicit Cmd_BindIndexBuffer(const core::SharedPtr<Buffer>& buffer_, UInt64 offset_ = 0) : buffer(buffer_.get()), offset(offset_) {}

    Buffer* buffer;
    UInt64  offset = 0;
};

struct Cmd_DrawIndexed final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override { return RHICommandType::DrawIndexed; }
    uint32_t                     index_count    = 0;
    uint32_t                     instance_count = 0;
    explicit Cmd_DrawIndexed(uint32_t index_count_, uint32_t instance_count_ = 1) : index_count(index_count_), instance_count(instance_count_) {}
};

struct Cmd_BindPipeline final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override { return RHICommandType::BindGraphicsPipeline; }

    GraphicsPipeline* pipeline;

    explicit Cmd_BindPipeline(GraphicsPipeline* pipeline_) : pipeline(pipeline_) {}
};

struct RenderAttachment
{
    ImageView*               target;
    ImageLayout              layout;
    AttachmentLoadOperation  load_op     = AttachmentLoadOperation::Clear;
    AttachmentStoreOperation store_op    = AttachmentStoreOperation::Store;
    core::Color              clear_color = core::Color::Green();
};

struct Cmd_BeginRender final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override { return RHICommandType::BeginRender; }

    Cmd_BeginRender(const core::Array<RenderAttachment>& colors_, const RenderAttachment& depth_, core::Size2D size_);

    core::Array<RenderAttachment> colors;
    RenderAttachment              depth;
    core::Size2D                  render_size;
};

struct Cmd_EndRender final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override { return RHICommandType::EndRender; }
};


struct Cmd_BeginRenderPass final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override { return RHICommandType::BeginRenderPass; }
};

struct Cmd_EndRenderPass final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override { return RHICommandType::EndRenderPass; }
};

}   // namespace platform::rhi

template <typename T, typename... Args>
T* NewRHICommand(Args&&... args)
{
    return NewDoubleFrameTemp<T>(Forward<Args>(args)...);
}
