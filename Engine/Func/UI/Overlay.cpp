//
// Created by Echo on 25-2-1.
//

#include "Overlay.h"

#include "Platform/RHI/Buffer.h"
#include "Platform/RHI/CommandBuffer.h"
#include "Platform/RHI/Commands.h"
#include "Platform/RHI/GfxCommandHelper.h"
#include "Platform/RHI/GfxContext.h"
#include "Platform/RHI/VertexLayout.h"
#include "Widget/Widget.h"

using namespace func;
using namespace ui;
using namespace widget;
using namespace platform;
using namespace rhi;

Overlay &Overlay::SetSlot(Widget *widget) {
  if (widget == nullptr) {
    return *this;
  } else {
    slot_ = widget;
    slot_->SetDirty();
    return *this;
  }
}

Overlay::~Overlay() {}

void Overlay::Draw(CommandBuffer &cmd) {
  if (!slot_)
    return;
  if (slot_->IsDirty() || dirty_) {
    core::Array<Vertex_UI> vertexs;
    core::Array<UInt32> indices;
    vertexs.reserve(500);
    indices.reserve(500);
    core::Rect2D target{};
    target.position.x = position_.x;
    target.position.y = position_.y;
    target.size.x = size_.x;
    target.size.y = size_.y;
    slot_->Rebuild(target, vertexs, indices);
    UInt64 vertex_buffer_size = sizeof(Vertex_UI) * vertexs.size();
    if (vertex_buffer_ == nullptr || vertex_buffer_->GetSize() < vertex_buffer_size) {
      BufferDesc buffer_desc{vertex_buffer_size, BUB_VertexBuffer | BUB_TransferDst, BMPB_DeviceLocal};
      vertex_buffer_ = GetGfxContext()->CreateBuffer(buffer_desc, "OverlayVertexBuffer");
    }
    GfxCommandHelper::CopyDataToBuffer(vertexs.data(), vertex_buffer_.get(), vertex_buffer_size);
    UInt64 index_buffer_size = sizeof(UInt32) * indices.size();
    if (index_buffer_ == nullptr || index_buffer_->GetSize() < index_buffer_size) {
      BufferDesc buffer_desc{index_buffer_size, BUB_IndexBuffer | BUB_TransferDst, BMPB_DeviceLocal};
      index_buffer_ = GetGfxContext()->CreateBuffer(buffer_desc, "OverlayIndexBuffer");
    }
    GfxCommandHelper::CopyDataToBuffer(indices.data(), index_buffer_.get(), index_buffer_size);
    dirty_ = false;
  }
  cmd.Enqueue<Cmd_BindVertexBuffer>(vertex_buffer_.get());
  cmd.Enqueue<Cmd_BindIndexBuffer>(index_buffer_.get());
  slot_->Draw(cmd);
}

void Overlay::SetPosition(core::Vector3 position) {
  position_ = position;
  SetDirty();
}

void Overlay::SetSize(core::Vector2 size) {
  size_ = size;
  SetDirty();
}