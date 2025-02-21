//
// Created by Echo on 25-2-1.
//

#pragma once

#include "Core/Math/MathTypes.h"
#include "Core/Object/Object.h"

#include GEN_HEADER("Func.Widget.generated.h")

namespace platform::rhi {
class CommandBuffer;
}
namespace func::ui {
class ISlotContainer;
}
namespace platform::rhi {
struct Vertex_UI;
}
namespace platform::rhi {
class Buffer;
}
namespace func::ui::widget {

class CLASS() Widget : public core::Object {
  GENERATED_CLASS(Widget)
public:
  Widget() : Object(core::ObjectFlagType::OFT_Widget) {}

  /**
   * 重建此Widget的顶点和索引缓冲数据
   */
  virtual void Rebuild(core::Rect2DI target_rect, core::Array<platform::rhi::Vertex_UI> &vertex_buffer,
                       core::Array<UInt32> &index_buffer) {
    NoEntry();
  }

  /**
   * 调用绘制命令, 此时已经完成了顶点和索引缓冲数据的填充
   * @param cmd
   */
  virtual void Draw(platform::rhi::CommandBuffer &cmd) { NoEntry(); }

  virtual core::Rect2DI GetBoundingRect() {
    NoEntry();
    return {};
  }

  void SetDirty(bool dirty = true);
  [[nodiscard]] bool IsDirty() const { return dirty_; }
  void SetVisible(bool visible = true);
  [[nodiscard]] bool IsVisible() const { return visible_; }

  [[nodiscard]] core::Vector4I GetPadding() const { return padding_; }
  void SetPadding(core::Vector4I padding);

protected:
  // x: left, y: top, z: right, w: bottom
  core::Vector4I padding_{};

  bool dirty_ = true;
  bool visible_ = true;
};

} // namespace func::ui::widget
