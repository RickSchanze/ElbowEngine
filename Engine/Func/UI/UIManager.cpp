//
// Created by Echo on 25-2-23.
//

#include "UIManager.h"

#include "Core/Object/ObjectRegistry.h"
#include "Platform/RHI/Buffer.h"
#include "Platform/RHI/GfxContext.h"
#include "Platform/RHI/VertexLayout.h"
#include "Widget/WindowPanel.h"

using namespace func;
using namespace ui;
using namespace platform;
using namespace rhi;

constexpr UInt64 PRESERVED_VERTEX_SIZE = 10000;
constexpr UInt64 PRESERVED_INDEX_SIZE = 30000;

void UIManager::Startup() {
  // 预先分配一万个UI顶点内存，以及三万个Index内存
  UInt64 vertex_buffer_size = PRESERVED_VERTEX_SIZE * sizeof(Vertex_UI);
  BufferDesc desc{vertex_buffer_size, BUB_VertexBuffer, BMPB_HostVisible | BMPB_HostCoherent};
  vertex_buffer_ = GetGfxContextRef().CreateBuffer(desc, "UIVertexBuffer");
  vertex_buffer_data_ = (Vertex_UI *)vertex_buffer_->BeginWrite();

  UInt64 index_buffer_size = PRESERVED_INDEX_SIZE * sizeof(UInt16);
  BufferDesc index_desc{index_buffer_size, BUB_IndexBuffer, BMPB_HostVisible | BMPB_HostCoherent};
  index_buffer_ = GetGfxContextRef().CreateBuffer(index_desc, "UIIndexBuffer");
  index_buffer_->BeginWrite();
  index_buffer_data_ = (UInt32 *)index_buffer_->BeginWrite();
}

void UIManager::Shutdown() {
  index_buffer_data_ = nullptr;
  index_buffer_->EndWrite();
  index_buffer_ = nullptr;

  vertex_buffer_data_ = nullptr;
  vertex_buffer_->EndWrite();
  vertex_buffer_ = nullptr;
}

void UIManager::Draw(CommandBuffer &cmd) {
  // TODO: 遮挡关系检查
  for (auto &panel : windows_) {
    widget::WindowPanel *p = core::ObjectManager::GetObjectByHandle<widget::WindowPanel>(panel);
    if (p) {
      core::ArrayProxy vertices = {vertex_buffer_data_, PRESERVED_VERTEX_SIZE};
      core::ArrayProxy indices = {index_buffer_data_, PRESERVED_INDEX_SIZE};
      core::Rect2DI draw_rect = p->GetBoundingRect();
      p->Rebuild(draw_rect, vertices, indices);
      p->Draw(cmd);
    } else {
      LOGGER.Error("UIManager", "Draw: 窗口{}不存在", panel);
    }
  }
}