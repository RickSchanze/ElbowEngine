//
// Created by Echo on 25-2-1.
//

#pragma once

#include "Core/Base/ArrayProxy.h"
#include "Core/Math/MathTypes.h"
#include "Core/Object/Object.h"
#include "Platform/RHI/VertexLayout.h"

#include GEN_HEADER("Func.Widget.generated.h")

#define APPLY_SCALE(x) (x) * ::func::ui::Style::GetGlobalScale()

namespace platform::rhi {
class CommandBuffer;
}
namespace func::ui {
class ISlotContainer;
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
   * 调用绘制命令, 此时已经完成了顶点和索引缓冲数据的填充
   * @param cmd
   */
  virtual void Draw(platform::rhi::CommandBuffer &cmd) { NoEntry(); }

  virtual void Rebuild(core::Rect2DI draw_rect);

  void SetDirty(bool dirty = true);
  [[nodiscard]] bool IsDirty() const { return dirty_; }
  void SetVisible(bool visible = true);
  [[nodiscard]] bool IsVisible() const { return visible_; }

  [[nodiscard]] core::Vector4I GetPadding() const { return padding_; }
  void SetPadding(core::Vector4I padding);

protected:
  UInt64 GetIndexOffset() const { return index_offset_; }
  UInt64 GetIndexSize() const { return index_size_; }

  // x: left, y: top, z: right, w: bottom
  core::Vector4I padding_{};

  UInt64 index_offset_ = 0;
  UInt64 index_size_ = 0;

  bool dirty_ = true;
  bool visible_ = true;
};

} // namespace func::ui::widget
