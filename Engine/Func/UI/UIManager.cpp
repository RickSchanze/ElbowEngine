//
// Created by Echo on 25-2-23.
//

#include "UIManager.h"

#include "Core/Base/Ranges.h"
#include "Core/Object/ObjectRegistry.h"
#include "Platform/RHI/Buffer.h"
#include "Platform/RHI/CommandBuffer.h"
#include "Platform/RHI/Commands.h"
#include "Platform/RHI/GfxContext.h"
#include "Platform/RHI/VertexLayout.h"
#include "Platform/Window/Window.h"
#include "Platform/Window/WindowManager.h"
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
  vertex_buffer_data_ = reinterpret_cast<Vertex_UI *>(vertex_buffer_->BeginWrite());

  UInt64 index_buffer_size = PRESERVED_INDEX_SIZE * sizeof(UInt16);
  BufferDesc index_desc{index_buffer_size, BUB_IndexBuffer, BMPB_HostVisible | BMPB_HostCoherent};
  index_buffer_ = GetGfxContextRef().CreateBuffer(index_desc, "UIIndexBuffer");
  index_buffer_data_ = reinterpret_cast<UInt32 *>(index_buffer_->BeginWrite());
}

void UIManager::Shutdown() {
  index_buffer_data_ = nullptr;
  index_buffer_->EndWrite();
  index_buffer_ = nullptr;

  vertex_buffer_data_ = nullptr;
  vertex_buffer_->EndWrite();
  vertex_buffer_ = nullptr;
}

void UIManager::Draw(CommandBuffer &cmd) { GetByRef().InternalDraw(cmd); }
void UIManager::AddWindow(widget::WindowPanel *window) { GetByRef().InternalAddWindow(window); }

Int32 UIManager::GetGlobalUIWidth() { return WindowManager::GetByRef().GetMainWindow()->GetWidth(); }
Int32 UIManager::GetGlobalUIHeight() { return WindowManager::GetByRef().GetMainWindow()->GetHeight(); }

void UIManager::InternalAddWindow(widget::WindowPanel *window) {
  if (!windows_.contains(window->GetHandle())) {
    windows_.insert(window->GetHandle());
  }
}

static UInt64 FindAvailableOffset(core::HashMap<core::ObjectHandle, OccupiedMemory> &occupied_vertex_,
                                  UInt64 vertex_count) {

  if (occupied_vertex_.size() == 0) {
    return 0;
  }
  core::Array<OccupiedMemory> vertex;
  vertex.reserve(occupied_vertex_.size());
  for (auto &occupied_vertex : occupied_vertex_ | core::range::view::Values) {
    vertex.push_back(occupied_vertex);
  }
  core::range::Sort(vertex, [](const OccupiedMemory &a, const OccupiedMemory &b) { return a.offset < b.offset; });
  // 寻找不连续的空闲内存, 如果符合要求就那么分配
  UInt64 offset_mark = 0;
  for (Int32 i = 0; i < vertex.size(); i++) {
    if (vertex[i].offset != offset_mark) {
      UInt64 free_size = vertex[i].offset - offset_mark;
      if (free_size >= vertex_count) {
        return offset_mark;
      }
    } else {
      offset_mark += vertex[i].size;
    }
  }
  return offset_mark;
}

VertexWriteData UIManager::RequestVertexWriteData(core::ObjectHandle handle, UInt64 vertex_count, UInt64 index_count) {
  return GetByRef().InternalRequestVertexWriteData(handle, vertex_count, index_count);
}

void UIManager::RecycleVertexData(core::ObjectHandle handle) { return GetByRef().InternalRecycleVertexData(handle); }

VertexWriteData UIManager::InternalRequestVertexWriteData(core::ObjectHandle handle, UInt64 vertex_count,
                                                          UInt64 index_count) {
  VertexWriteData rtn{};
  if (occupied_vertex_.contains(handle)) {
    if (occupied_vertex_[handle].size >= vertex_count) {
      rtn.vertex_offset = occupied_vertex_[handle].offset;
      rtn.vertices = core::ArrayProxy(vertex_buffer_data_ + rtn.vertex_offset, vertex_count);
    } else {
      occupied_vertex_.erase(handle);
    }
  }
  if (rtn.vertices.Remain() == 0) {
    rtn.vertex_offset = FindAvailableOffset(occupied_vertex_, vertex_count);
    rtn.vertices = core::ArrayProxy(vertex_buffer_data_ + rtn.vertex_offset, vertex_count);
  }
  if (occupied_index_.contains(handle)) {
    if (occupied_index_[handle].size >= index_count) {
      rtn.index_offset = occupied_index_[handle].offset;
      rtn.indices = core::ArrayProxy(index_buffer_data_ + rtn.index_offset, index_count);
    } else {
      occupied_index_.erase(handle);
    }
  }
  if (rtn.indices.Remain() == 0) {
    rtn.index_offset = FindAvailableOffset(occupied_index_, index_count);
    rtn.indices = core::ArrayProxy(index_buffer_data_ + rtn.index_offset, index_count);
  }
  occupied_vertex_[handle] = {rtn.vertex_offset, vertex_count};
  occupied_index_[handle] = {rtn.index_offset, index_count};
  return rtn;
}

void UIManager::InternalRecycleVertexData(core::ObjectHandle handle) {
  if (occupied_vertex_.contains(handle)) {
    occupied_vertex_.erase(handle);
  }
  if (occupied_index_.contains(handle)) {
    occupied_index_.erase(handle);
  }
}

void UIManager::InternalDraw(CommandBuffer &cmd) {
  // TODO: 遮挡关系检查
  cmd.Enqueue<Cmd_BindVertexBuffer>(vertex_buffer_.get());
  cmd.Enqueue<Cmd_BindIndexBuffer>(index_buffer_.get());
  for (auto &panel : windows_) {
    widget::WindowPanel *p = core::ObjectManager::GetObjectByHandle<widget::WindowPanel>(panel);
    if (p) {
      core::Rect2DI draw_rect = p->GetBoundingRect();
      p->Rebuild(draw_rect);
      p->Draw(cmd);
    } else {
      LOGGER.Error("UIManager", "Draw: 窗口{}不存在", panel);
    }
  }
}
