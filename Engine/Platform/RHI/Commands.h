//
// Created by Echo on 24-12-21.
//

#pragma once
#include "Core/CoreGlobal.h"
#include "Core/Math/MathTypes.h"
#include "Core/Memory/MemoryManager.h"
#include "Enums.h"

namespace platform::rhi {
class DescriptorSet;
class Image;
} // namespace platform::rhi
namespace platform::rhi {
class ImageView;
class GraphicsPipeline;
class Buffer;
} // namespace platform::rhi
namespace platform::rhi {

enum class RHICommandType {
  CopyBuffer,
  BindVertexBuffer,
  BindIndexBuffer,
  DrawIndexed,
  BindGraphicsPipeline,
  BeginRender,
  EndRender,
  BeginRenderPass,
  EndRenderPass,
  ImagePipelineBarrier,
  SetScissor,
  SetViewport,
  BindDescriptorSet,
  CopyBufferToImage,
  Count,
};

struct RHICommand {
  virtual ~RHICommand() = default;
  [[nodiscard]] virtual RHICommandType GetType() const = 0;
};

struct Cmd_CopyBuffer final : RHICommand {
  [[nodiscard]] RHICommandType GetType() const override { return RHICommandType::CopyBuffer; }

  Buffer *src;
  Buffer *dst;
  size_t size = 0;

  Cmd_CopyBuffer(Buffer *src_, Buffer *dst_, const size_t size_ = 0) : src(src_), dst(dst_), size(size_) {}
};

struct Cmd_BindVertexBuffer final : RHICommand {
  [[nodiscard]] RHICommandType GetType() const override { return RHICommandType::BindVertexBuffer; }

  explicit Cmd_BindVertexBuffer(Buffer *buffer_, UInt64 offset_ = 0, UInt32 binding_ = 0)
      : binding(binding_), buffer(buffer_), offset(offset_) {}

  explicit Cmd_BindVertexBuffer(const core::SharedPtr<Buffer> &buffer_, UInt64 offset_ = 0, UInt32 binding_ = 0)
      : binding(binding_), buffer(buffer_.get()), offset(offset_) {}

  UInt32 binding;
  Buffer *buffer;
  UInt64 offset = 0;
};

struct Cmd_BindIndexBuffer final : RHICommand {
  [[nodiscard]] RHICommandType GetType() const override { return RHICommandType::BindIndexBuffer; }

  explicit Cmd_BindIndexBuffer(Buffer *buffer_, UInt64 offset_ = 0) : buffer(buffer_), offset(offset_) {}
  explicit Cmd_BindIndexBuffer(const core::SharedPtr<Buffer> &buffer_, UInt64 offset_ = 0)
      : buffer(buffer_.get()), offset(offset_) {}

  Buffer *buffer;
  UInt64 offset = 0;
};

struct Cmd_DrawIndexed final : RHICommand {
  [[nodiscard]] RHICommandType GetType() const override { return RHICommandType::DrawIndexed; }
  uint32_t index_count = 0;
  uint32_t instance_count = 0;
  UInt32 first_index = 0;
  explicit Cmd_DrawIndexed(uint32_t index_count_, uint32_t instance_count_ = 1, UInt32 first_index_)
      : index_count(index_count_), instance_count(instance_count_), first_index(first_index_) {}
};

struct Cmd_BindPipeline final : RHICommand {
  [[nodiscard]] RHICommandType GetType() const override { return RHICommandType::BindGraphicsPipeline; }

  GraphicsPipeline *pipeline;

  explicit Cmd_BindPipeline(GraphicsPipeline *pipeline_) : pipeline(pipeline_) {}
};

struct Cmd_BindDescriptorSet final : RHICommand {
  [[nodiscard]] RHICommandType GetType() const override { return RHICommandType::BindDescriptorSet; }
  GraphicsPipeline *pipeline;
  DescriptorSet *set;
  explicit Cmd_BindDescriptorSet(GraphicsPipeline *pipeline_, DescriptorSet *set_) : pipeline(pipeline_), set(set_) {}
};

struct RenderAttachment {
  ImageView *target;
  ImageLayout layout;
  AttachmentLoadOperation load_op = AttachmentLoadOperation::Clear;
  AttachmentStoreOperation store_op = AttachmentStoreOperation::Store;
  core::Color clear_color = core::Color::Green();
};

struct Cmd_BeginRender final : RHICommand {
  [[nodiscard]] RHICommandType GetType() const override { return RHICommandType::BeginRender; }

  explicit Cmd_BeginRender(const core::PooledArray<RenderAttachment> &colors_, const RenderAttachment &depth_ = {},
                           core::Size2D size_ = {0, 0});

  core::PooledArray<RenderAttachment> colors;
  RenderAttachment depth;
  core::Size2D render_size{};
};

struct Cmd_EndRender final : RHICommand {
  [[nodiscard]] RHICommandType GetType() const override { return RHICommandType::EndRender; }
};

struct Cmd_ImagePipelineBarrier final : RHICommand {
  [[nodiscard]] RHICommandType GetType() const override { return RHICommandType::ImagePipelineBarrier; }

  /**
   * 参数解释－VkImageMemoryBarrier
   * @param old_layout_
   * @param new_layout_
   * @param target_
   * @param subresource_range_
   * @param src_access_
   * @param dst_access_
   */
  explicit Cmd_ImagePipelineBarrier(ImageLayout old_layout_, ImageLayout new_layout_, Image *target_,
                                    const ImageSubresourceRange &subresource_range_, AccessFlags src_access_,
                                    AccessFlags dst_access_, PipelineStageFlags src_stage_,
                                    PipelineStageFlags dst_stage_)
      : old_layout(old_layout_), new_layout(new_layout_), target(target_), subresource_range(subresource_range_),
        src_access(src_access_), dst_access(dst_access_), src_stage(src_stage_), dst_stage(dst_stage_) {}

  ImageLayout old_layout;
  ImageLayout new_layout;
  Image *target;
  ImageSubresourceRange subresource_range;
  AccessFlags src_access;
  AccessFlags dst_access;
  PipelineStageFlags src_stage;
  PipelineStageFlags dst_stage;
};

struct Cmd_BeginRenderPass final : RHICommand {
  [[nodiscard]] RHICommandType GetType() const override { return RHICommandType::BeginRenderPass; }
};

struct Cmd_EndRenderPass final : RHICommand {
  [[nodiscard]] RHICommandType GetType() const override { return RHICommandType::EndRenderPass; }
};

struct Cmd_SetScissor final : RHICommand {
  [[nodiscard]] RHICommandType GetType() const override { return RHICommandType::SetScissor; }

  explicit Cmd_SetScissor(const core::Rect2D &scissor_) : scissor(scissor_) {}

  core::Rect2D scissor;
};

struct Cmd_SetViewport final : RHICommand {
  [[nodiscard]] RHICommandType GetType() const override { return RHICommandType::SetViewport; }

  explicit Cmd_SetViewport(const core::Rect2D &viewport_) : viewport(viewport_) {}

  core::Rect2D viewport;
};

struct Cmd_CopyBufferToImage final : RHICommand {
  [[nodiscard]] RHICommandType GetType() const override { return RHICommandType::CopyBufferToImage; }
  explicit Cmd_CopyBufferToImage(Buffer *src_, Image *dst_, ImageSubresourceRange subresource_range_,
                                 core::Vector3i offset_, core::Vector3i size_)
      : src(src_), dst(dst_), subresource_range(subresource_range_), offset(offset_), size(size_) {}

  Buffer *src = nullptr;
  Image *dst = nullptr;
  ImageSubresourceRange subresource_range{};
  core::Vector3i offset{};
  core::Vector3i size{};
};

} // namespace platform::rhi
