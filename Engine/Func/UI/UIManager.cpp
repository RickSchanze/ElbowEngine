//
// Created by Echo on 25-2-23.
//

#include "UIManager.h"

#include "Core/Base/Ranges.h"
#include "Core/Math/Math.h"
#include "Core/Object/ObjectRegistry.h"
#include "Func/Input/Input.h"
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
using namespace core;
using namespace widget;

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

  input_event_id_ = Input::FrameInputEvent.AddBind(this, &UIManager::InternalProcessInput);
}

void UIManager::Shutdown() {
  Input::FrameInputEvent.RemoveBind(input_event_id_);
  index_buffer_data_ = nullptr;
  index_buffer_->EndWrite();
  index_buffer_ = nullptr;

  vertex_buffer_data_ = nullptr;
  vertex_buffer_->EndWrite();
  vertex_buffer_ = nullptr;
}

void UIManager::Draw(CommandBuffer &cmd) { GetByRef().InternalDraw(cmd); }
void UIManager::AddWindow(WindowPanel *window) { GetByRef().InternalAddWindow(window); }

Int32 UIManager::GetGlobalUIWidth() { return WindowManager::GetMainWindow()->GetWidth(); }
Int32 UIManager::GetGlobalUIHeight() { return WindowManager::GetMainWindow()->GetHeight(); }

void UIManager::InternalAddWindow(WindowPanel *window) {
  if (!windows_handles_.contains(window->GetHandle())) {
    windows_handles_.insert(window->GetHandle());
  }
}

void UIManager::InternalProcessInput(const InputEventParam &event) {
  PROFILE_SCOPE_AUTO;
  // 输入的鼠标位置左上角是原点, 给他改成左下角原点的
  Vector2I mouse_pos = {event.mouse_pos.x, GetGlobalUIHeight() - event.mouse_pos.y};
  // 优先对焦点窗口处理
  // 键盘事件直接发送给焦点窗口
  if (focused_window_panel_handle_ != 0) {
    auto *panel = ObjectManager::GetObjectByHandle<WindowPanel>(focused_window_panel_handle_);
    panel->OnKeyUp(event.released_keys);
    panel->OnKeyDown(event.pressed_keys);
    // 鼠标相关事件, 查询当前鼠标是否在焦点窗口, 如果在则发送给焦点窗口
    Rect2DI bounding = panel->GetBoundingRect();
    if (Math::IsPointInsideRect(mouse_pos, bounding)) {
      if (Input::HasKeyRespond(event.pressed_keys)) {
        panel->OnKeyDown(event.pressed_keys);
      }
      if (Input::HasKeyRespond(event.released_keys)) {
        panel->OnKeyUp(event.released_keys);
      }
      if (Input::HasMouseButtonRespond(event.pressed_mouse)) {
        // 将x, y转换至窗口的本地坐标
        Vector2I new_pos = Math::TransformCoord(mouse_pos, panel->GetBoundingRect().LeftBottom());
        panel->OnMousePressed(event.pressed_mouse, new_pos.x, new_pos.y);
      }
      if (Input::HasMouseButtonRespond(event.released_mouse)) {
        // 将x, y转换至窗口的本地坐标
        Vector2I new_pos = Math::TransformCoord(mouse_pos, panel->GetBoundingRect().LeftBottom());
        panel->OnMouseReleased(event.released_mouse, new_pos.x, new_pos.y);
      }
      if (!Math::IsNearlyZero(event.mouse_move)) {
        panel->OnMouseMove(event.mouse_move.x, event.mouse_move.y);
      }
      if (!Math::IsNearlyZero(event.mouse_scroll)) {
        panel->OnMouseScroll(event.mouse_scroll.x, event.mouse_scroll.y);
      }
      return;
    }
  }
  bool is_point_in_window = false;
  // 走到这说明鼠标不在焦点窗口, 并且键盘事件只发送给焦点窗口
  for (auto &window_handle : windows_handles_) {
    if (window_handle == focused_window_panel_handle_)
      continue;
    auto *panel = ObjectManager::GetObjectByHandle<WindowPanel>(window_handle);
    if (Math::IsPointInsideRect(mouse_pos, panel->GetBoundingRect())) {
      // Move和Scroll事件不关心是哪个窗口
      // TODO: 应该只触发顶层窗口
      // TODO: 想办法找到顶层窗口
      if (!Math::IsNearlyZero(event.mouse_move)) {
        panel->OnMouseMove(event.mouse_move.x, event.mouse_move.y);
      }
      if (!Math::IsNearlyZero(event.mouse_scroll)) {
        panel->OnMouseScroll(event.mouse_scroll.x, event.mouse_scroll.y);
      }
      // 在非focused窗口按下左键, 则修改focused
      if (Input::IsMouseButtonPressed(MouseButton::Left, event.pressed_mouse)) {
        pressed_window_panel_handle_ = panel->GetHandle();
      }
      if (Input::IsMouseButtonReleased(MouseButton::Left, event.pressed_mouse)) {
        if (panel->GetHandle() == pressed_window_panel_handle_) {
          if (WindowPanel *old_focused_window =
                  ObjectManager::GetObjectByHandle<WindowPanel>(focused_window_panel_handle_)) {
            old_focused_window->SetFocused(false);
          }
          panel->SetFocused(true);
          focused_window_panel_handle_ = panel->GetHandle();
        }
        return;
      }
      is_point_in_window = true;
    }
  }
  // 按到的地方没有Window, 并且有鼠标左键点击释放, 那么取消window的focus
  if (!is_point_in_window && Input::IsMouseButtonReleased(MouseButton::Left, event.released_mouse)) {
    if (WindowPanel *old_focused_window = ObjectManager::GetObjectByHandle<WindowPanel>(focused_window_panel_handle_)) {
      old_focused_window->SetFocused(false);
    }
    focused_window_panel_handle_ = 0;
  }
}

static UInt64 FindAvailableOffset(HashMap<ObjectHandle, OccupiedMemory> &occupied_vertex_, UInt64 vertex_count) {
  PROFILE_SCOPE_AUTO;
  if (occupied_vertex_.size() == 0) {
    return 0;
  }
  Array<OccupiedMemory> vertex;
  vertex.reserve(occupied_vertex_.size());
  for (auto &occupied_vertex : occupied_vertex_ | range::view::Values) {
    vertex.push_back(occupied_vertex);
  }
  range::Sort(vertex, [](const OccupiedMemory &a, const OccupiedMemory &b) { return a.offset < b.offset; });
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
  PROFILE_SCOPE_AUTO;
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

void UIManager::InternalDraw(CommandBuffer &cmd) const {
  PROFILE_SCOPE_AUTO;
  // TODO: 遮挡关系检查
  cmd.Enqueue<Cmd_BindVertexBuffer>(vertex_buffer_.get());
  cmd.Enqueue<Cmd_BindIndexBuffer>(index_buffer_.get());
  for (auto &panel : windows_handles_) {
    WindowPanel *p = core::ObjectManager::GetObjectByHandle<WindowPanel>(panel);
    if (p) {
      core::Rect2DI draw_rect = p->GetBoundingRect();
      p->Rebuild(draw_rect);
      p->Draw(cmd);
    } else {
      LOGGER.Error("UIManager", "Draw: 窗口{}不存在", panel);
    }
  }
}
