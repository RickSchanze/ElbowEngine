//
// Created by Echo on 25-2-23.
//

#pragma once
#include "Core/Object/Object.h"
#include "Core/Singleton/MManager.h"

namespace platform::rhi {
struct Vertex_UI;
}
namespace platform::rhi {
class CommandBuffer;
}
namespace platform::rhi {
class Buffer;
}
namespace func::ui::widget {
class WindowPanel;
}
namespace func::ui {

// 用于管理WindowPanel 以及输入 以及所有顶点
class UIManager : public core::Manager<UIManager> {
public:
  [[nodiscard]] core::ManagerLevel GetLevel() const override { return core::ManagerLevel::L8; }
  [[nodiscard]] core::StringView GetName() const override { return "UIManager"; }

  void Startup() override;
  void Shutdown() override;

  void Draw(platform::rhi::CommandBuffer &cmd);

  void AddWindow(widget::WindowPanel *window);

private:
  core::HashSet<core::ObjectHandle> windows_;

  core::SharedPtr<platform::rhi::Buffer> vertex_buffer_;
  core::SharedPtr<platform::rhi::Buffer> index_buffer_;
  platform::rhi::Vertex_UI *vertex_buffer_data_ = nullptr;
  UInt32 *index_buffer_data_ = nullptr;
};

} // namespace func::ui
