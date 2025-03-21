//
// Created by Echo on 25-2-23.
//

#pragma once
#include "Core/Base/ArrayProxy.h"
#include "Core/Object/Object.h"
#include "Core/Singleton/MManager.h"
#include "Func/Input/Input.h"
#include "Platform/RHI/VertexLayout.h"
#include "Platform/Window/Window.h"

namespace func::ui::widget {
class Text;
}
namespace resource {
class Material;
}
namespace platform::rhi {
class CommandBuffer;
class Buffer;
} // namespace platform::rhi
namespace func::ui::widget {
class WindowPanel;
}
namespace func::ui {

struct VertexWriteData {
  core::ArrayProxy<platform::rhi::Vertex_UI> vertices;
  UInt64 vertex_offset{};
  core::ArrayProxy<UInt32> indices;
  UInt64 index_offset{};
};

struct OccupiedMemory {
  UInt64 offset;
  UInt64 size;
};

// 用于管理WindowPanel 以及输入 以及所有顶点
class UIManager : public core::Manager<UIManager> {
public:
  [[nodiscard]] core::ManagerLevel GetLevel() const override { return core::ManagerLevel::L8; }
  [[nodiscard]] core::StringView GetName() const override { return "UIManager"; }

  void Startup() override;
  void Shutdown() override;

  static VertexWriteData RequestVertexWriteData(core::ObjectHandle handle, UInt64 vertex_count, UInt64 index_count);
  static void RecycleVertexData(core::ObjectHandle handle);
  static void Draw(platform::rhi::CommandBuffer &cmd);
  static void AddWindow(widget::WindowPanel *window);
  static Int32 GetGlobalUIWidth();
  static Int32 GetGlobalUIHeight();
  static resource::Material *GetDefaultUIMaterial();
  static void AddDrawText(widget::Text* t);

private:
  VertexWriteData InternalRequestVertexWriteData(core::ObjectHandle handle, UInt64 vertex_count, UInt64 index_count);
  void InternalRecycleVertexData(core::ObjectHandle handle);
  void InternalDraw(platform::rhi::CommandBuffer &cmd);
  void InternalAddWindow(widget::WindowPanel *window);
  void InternalProcessInput(const InputEventParam &event);
  void InternalAddDrawText(widget::Text* t);

  core::HashSet<core::ObjectHandle> windows_handles_;

  core::SharedPtr<platform::rhi::Buffer> vertex_buffer_;
  core::SharedPtr<platform::rhi::Buffer> index_buffer_;
  platform::rhi::Vertex_UI *vertex_buffer_data_ = nullptr;
  UInt32 *index_buffer_data_ = nullptr;

  core::HashMap<core::ObjectHandle, OccupiedMemory> occupied_vertex_;
  core::HashMap<core::ObjectHandle, OccupiedMemory> occupied_index_;

  core::DelegateID input_event_id_;
  core::ObjectHandle focused_window_panel_handle_ = 0;
  // 用于确定Focused window
  core::ObjectHandle pressed_window_panel_handle_ = 0; // 用于处理即将按下, 即focused改变的情况
  // 鼠标在哪个窗口?
  core::ObjectHandle mouse_stay_window_handle_ = 0;
  // 收集所有Text, 用于一次渲染, 这样不需要重复绑定DescriptorSet
  core::HashSet<widget::Text *> texts_to_render_;
};

} // namespace func::ui
