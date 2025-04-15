//
// Created by Echo on 2025/3/22.
//
#pragma once
#include "Core/Core.hpp"
#include "Core/Math/Types.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "Enums.hpp"


class SharedMaterial;
class Material;

namespace RHI
{
class GraphicsPipeline;
}

namespace RHI
{
class Buffer;
class DescriptorSet;
class Image;
class ImageView;
class Pipeline;

enum class RHICommandType
{
    CopyBuffer,
    BindVertexBuffer,
    BindIndexBuffer,
    DrawIndexed,
    BindGraphicsPipeline,
    BindComputePipeline,
    BeginRender,
    EndRender,
    BeginRenderPass,
    EndRenderPass,
    ImagePipelineBarrier,
    SetScissor,
    SetViewport,
    BindDescriptorSet,
    BindDescriptorSetCompute,
    CopyBufferToImage,
    CopyImageToBuffer,
    BeginCommandDebugLabel,
    EndCommandDebugLabel,
    BlitImage,
    Dispatch,
    PushConstant,
    Count,
};

struct RHICommand
{
    virtual ~RHICommand() = default;

    [[nodiscard]] virtual RHICommandType GetType() const = 0;
};


struct Cmd_BeginCommandLabel final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override
    {
        return RHICommandType::BeginCommandDebugLabel;
    }

    StringView label;

    explicit Cmd_BeginCommandLabel(StringView label_) : label(label_)
    {
    }
};

struct Cmd_EndCommandDebugLabel final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override
    {
        return RHICommandType::EndCommandDebugLabel;
    }
};

struct Cmd_PushConstant final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override
    {
        return RHICommandType::PushConstant;
    }

    explicit Cmd_PushConstant(Pipeline *InPipeline, const UInt32 InOffset, const UInt32 InSize, void *InData) : mPipeline(InPipeline),
        mOffset(InOffset), mSize(InSize), mData(InData)
    {
    }

    ~Cmd_PushConstant() override
    {
        Delete(mData);
    }

    Pipeline *mPipeline;
    UInt32 mOffset;
    UInt32 mSize;
    void *mData;
};

struct Cmd_CopyBuffer final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override
    {
        return RHICommandType::CopyBuffer;
    }

    Buffer *src;
    Buffer *dst;
    size_t size = 0;

    Cmd_CopyBuffer(Buffer *src_, Buffer *dst_, const size_t size_ = 0) : src(src_), dst(dst_), size(size_)
    {
    }
};

struct Cmd_BindVertexBuffer final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override
    {
        return RHICommandType::BindVertexBuffer;
    }

    explicit Cmd_BindVertexBuffer(Buffer *buffer_, const UInt64 offset_ = 0, const UInt32 binding_ = 0) : binding(binding_), buffer(buffer_),
        offset(offset_)
    {
    }

    explicit Cmd_BindVertexBuffer(const SharedPtr<Buffer> &buffer_, const UInt64 offset_ = 0, const UInt32 binding_ = 0) : binding(binding_),
        buffer(buffer_.get()), offset(offset_)
    {
    }

    UInt32 binding;
    Buffer *buffer;
    UInt64 offset = 0;
};

struct Cmd_BindIndexBuffer final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override
    {
        return RHICommandType::BindIndexBuffer;
    }

    explicit Cmd_BindIndexBuffer(Buffer *buffer_, const UInt64 offset_ = 0) : buffer(buffer_), offset(offset_)
    {
    }

    explicit Cmd_BindIndexBuffer(const SharedPtr<Buffer> &buffer_, const UInt64 offset_ = 0) : buffer(buffer_.get()), offset(offset_)
    {
    }

    Buffer *buffer;
    UInt64 offset = 0;
};

struct Cmd_DrawIndexed final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override
    {
        return RHICommandType::DrawIndexed;
    }

    UInt32 index_count = 0;
    UInt32 instance_count = 0;
    UInt32 first_index = 0;

    DEBUG_ONLY(String debug_name_ = nullptr);

    explicit Cmd_DrawIndexed(const uint32_t index_count_, const uint32_t instance_count_ = 1,
                             const UInt32 first_index_ = 0 DEBUG_ONLY(, const String &debug_name = "")) : index_count(index_count_),
        instance_count(instance_count_), first_index(first_index_) DEBUG_ONLY(, debug_name_(debug_name))
    {
    }
};

struct Cmd_BindPipeline final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override
    {
        return RHICommandType::BindGraphicsPipeline;
    }

    Pipeline *pipeline;

    explicit Cmd_BindPipeline(Pipeline *pipeline_) : pipeline(pipeline_)
    {
    }
};

struct Cmd_BindComputePipeline final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override
    {
        return RHICommandType::BindComputePipeline;
    }

    Pipeline *pipeline;

    explicit Cmd_BindComputePipeline(Pipeline *pipeline_) : pipeline(pipeline_)
    {
    }
};

struct Cmd_BindDescriptorSet final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override
    {
        return RHICommandType::BindDescriptorSet;
    }

    Pipeline *pipeline;
    DescriptorSet *set;

    explicit Cmd_BindDescriptorSet(Pipeline *pipeline_, DescriptorSet *set_) : pipeline(pipeline_), set(set_)
    {
    }
};

struct Cmd_BindDescriptorSetCompute final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override
    {
        return RHICommandType::BindDescriptorSetCompute;
    }

    Pipeline *pipeline;
    DescriptorSet *set;

    explicit Cmd_BindDescriptorSetCompute(Pipeline *pipeline_, DescriptorSet *set_) : pipeline(pipeline_), set(set_)
    {
    }
};

struct RenderAttachment
{
    ImageView *target;
    ImageLayout layout;
    AttachmentLoadOperation load_op = AttachmentLoadOperation::Clear;
    AttachmentStoreOperation store_op = AttachmentStoreOperation::Store;
    Color clear_color = Color::Green();
};

struct Cmd_BeginRender final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override
    {
        return RHICommandType::BeginRender;
    }

    explicit Cmd_BeginRender(const Array<RenderAttachment> &colors_, const RenderAttachment &depth_ = {}, Vector2f size_ = {0, 0});

    Array<RenderAttachment> colors;
    RenderAttachment depth;
    Vector2f render_size{};
};

struct Cmd_EndRender final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override
    {
        return RHICommandType::EndRender;
    }
};

struct Cmd_ImagePipelineBarrier final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override
    {
        return RHICommandType::ImagePipelineBarrier;
    }

    /**
     * 参数解释－VkImageMemoryBarrier
     * @param old_layout_
     * @param new_layout_
     * @param target_
     * @param subresource_range_
     * @param src_access_
     * @param dst_access_
     * @param src_stage_
     * @param dst_stage_
     */
    explicit Cmd_ImagePipelineBarrier(const ImageLayout old_layout_, const ImageLayout new_layout_, Image *target_,
                                      const ImageSubresourceRange &subresource_range_, const AccessFlags src_access_,
                                      const AccessFlags dst_access_, const PipelineStageFlags src_stage_,
                                      const PipelineStageFlags dst_stage_) : old_layout(old_layout_), new_layout(new_layout_), target(target_),
                                                                             subresource_range(subresource_range_), src_access(src_access_),
                                                                             dst_access(dst_access_), src_stage(src_stage_), dst_stage(dst_stage_)
    {
    }

    ImageLayout old_layout;
    ImageLayout new_layout;
    Image *target;
    ImageSubresourceRange subresource_range;
    AccessFlags src_access;
    AccessFlags dst_access;
    PipelineStageFlags src_stage;
    PipelineStageFlags dst_stage;
};

struct Cmd_BeginRenderPass final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override
    {
        return RHICommandType::BeginRenderPass;
    }
};

struct Cmd_EndRenderPass final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override
    {
        return RHICommandType::EndRenderPass;
    }
};

struct Cmd_SetScissor final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override
    {
        return RHICommandType::SetScissor;
    }

    explicit Cmd_SetScissor(const Rect2Df &scissor_) : scissor(scissor_)
    {
    }

    Rect2Df scissor;
};

struct Cmd_SetViewport final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override
    {
        return RHICommandType::SetViewport;
    }

    explicit Cmd_SetViewport(const Rect2Df &viewport_) : viewport(viewport_)
    {
    }

    Rect2Df viewport;
};

struct Cmd_CopyBufferToImage final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override
    {
        return RHICommandType::CopyBufferToImage;
    }

    explicit Cmd_CopyBufferToImage(Buffer *src_, Image *dst_, const ImageSubresourceRange &subresource_range_, const Vector3i offset_,
                                   const Vector3i size_) : src(src_), dst(dst_), subresource_range(subresource_range_), offset(offset_), size(size_)
    {
    }

    Buffer *src = nullptr;
    Image *dst = nullptr;
    ImageSubresourceRange subresource_range{};
    Vector3i offset{};
    Vector3i size{};
};

struct Cmd_CopyImageToBuffer final : RHICommand
{
    RHICommandType GetType() const override
    {
        return RHICommandType::CopyImageToBuffer;
    }

    explicit Cmd_CopyImageToBuffer(Image *src_, Buffer *dst_, const ImageSubresourceRange &subresource_range_, const Vector3i offset_,
                                   const Vector3i size_) : src(src_), dst(dst_), subresource_range(subresource_range_), offset(offset_), size(size_)
    {
    }

    Image *src = nullptr;
    Buffer *dst = nullptr;
    ImageSubresourceRange subresource_range{};
    Vector3i offset{};
    Vector3i size{};
};

struct Cmd_Dispatch final : RHICommand
{
    [[nodiscard]] RHICommandType GetType() const override
    {
        return RHICommandType::Dispatch;
    }

    explicit Cmd_Dispatch(const UInt32 x_, const UInt32 y_, const UInt32 z_) : x(x_), y(y_), z(z_)
    {
    }

    UInt32 x = 0;
    UInt32 y = 0;
    UInt32 z = 0;
};

struct ImageBlitDesc
{
    ImageSubresourceRange src_subresource;
    Vector3i src_offsets[2];
    ImageSubresourceRange dst_subresource;
    Vector3i dst_offsets[2];
};

struct Cmd_BlitImage : RHICommand
{
    RHICommandType GetType() const override
    {
        return RHICommandType::BlitImage;
    }

    explicit Cmd_BlitImage(Image *src_, Image *dst_, const ImageBlitDesc &desc_) : src(src_), dst(dst_), desc(desc_)
    {
    }

    Image *src;
    Image *dst;
    ImageBlitDesc desc;
};
} // namespace rhi