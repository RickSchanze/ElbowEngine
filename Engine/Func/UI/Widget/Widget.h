//
// Created by Echo on 25-2-1.
//

#pragma once

#include "Core/Base/ArrayProxy.h"
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
class Buffer;
}
namespace func::ui::widget {

struct STRUCT() Padding {
  PROPERTY()
  float left = 0.f;

  PROPERTY()
  float top = 0.f;

  PROPERTY()
  float right = 0.f;

  PROPERTY()
  float bottom = 0.f;

  /**
   * 将rect表示的范围应用padding后获得的新范围
   * @param rect
   * @return
   */
  core::Rect2DI Apply(core::Rect2DI rect) const;
};

enum class ENUM() HorizontalAlignment {
  Left,
  Center,
  Right,
};

enum class ENUM() VerticalAlignment {
  Top,
  Center,
  Bottom,
};

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

  /**
   * 这个Widget是否接受输入 如果返回false那么计算输入响应时将忽略此Widget
   */
  [[nodiscard]] bool ReceiveInput() const { return receive_input_; }

  void SetReceiveInput(bool receive_input) { receive_input_ = receive_input; }

  void SetDirty(bool dirty = true);
  [[nodiscard]] bool IsDirty() const { return dirty_; }
  virtual void OnSetDirty() {}
  void SetVisible(bool visible = true);
  [[nodiscard]] bool IsVisible() const { return visible_; }

  // 此函数用于计算这个Widget的包围盒的大小
  virtual core::Vector2I GetBoundingSize() { return {}; }

  Widget *GetParent() const { return parent_; }
  void SetParent(Widget *parent) { parent_ = parent; }

protected:
  UInt64 GetIndexOffset() const { return index_offset_; }
  UInt64 GetIndexSize() const { return index_size_; }

  UInt64 index_offset_ = 0;
  UInt64 index_size_ = 0;

  bool dirty_ = true;
  bool visible_ = true;
  bool receive_input_ = false;
  Widget *parent_ = nullptr;
};

// 一些通用的帮助函数

/// 计算一个修正后的绘制矩形
/// 要在draw_rect中以vertical_alignment和horizontal_alignment对齐, 应该往哪绘制?
/// 此函数用于计算这个, 且draw_rect的size会永远等于输入size
core::Rect2DI CalcAlignedDrawRect(const core::Rect2DI &draw_rect, const core::Vector2I &size,
                                  HorizontalAlignment horizontal_alignment, VerticalAlignment vertical_alignment);

} // namespace func::ui::widget
