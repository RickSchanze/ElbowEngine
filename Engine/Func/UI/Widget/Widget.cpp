//
// Created by Echo on 25-2-1.
//

#include "Widget.h"

#include "Core/Reflection/CtorManager.h"
#include "Core/Reflection/Reflection.h"

#include GEN_HEADER("Func.Widget.generated.h")

GENERATED_SOURCE()

using namespace func;
using namespace ui;
using namespace widget;

core::Rect2DI Padding::Apply(core::Rect2DI rect) const {
  rect.size.x -= (left + right);
  rect.size.y -= (top + bottom);
  rect.position.x += left;
  rect.position.y += bottom;
  return rect;
}

void Widget::Rebuild(core::Rect2DI draw_rect) { NoEntry(); }
void Widget::SetDirty(bool dirty) {
  dirty_ = dirty;
  if (dirty_) {
    OnSetDirty();
  }
}

void Widget::SetVisible(bool visible) {
  if (visible != visible_) {
    visible_ = visible;
    SetDirty();
  }
}

core::Rect2DI widget::CalcAlignedDrawRect(const core::Rect2DI &draw_rect, const core::Vector2I &size,
                                          HorizontalAlignment horizontal_alignment,
                                          VerticalAlignment vertical_alignment) {
  core::Rect2DI adjusted_rect = draw_rect;
  adjusted_rect.size = size;
  // 处理vertical
  if (size.y < draw_rect.size.y) {
    switch (vertical_alignment) {
    case VerticalAlignment::Center:
      adjusted_rect.position.y += (draw_rect.size.y - size.y) / 2;
      break;
    case VerticalAlignment::Top:
      adjusted_rect.position.y += draw_rect.size.y - size.y;
      break;
    default:
      break;
    }
  }
  if (size.x < draw_rect.size.x) {
    switch (horizontal_alignment) {
    case HorizontalAlignment::Center:
      adjusted_rect.position.x += (draw_rect.size.x - size.x) / 2;
      break;
    case HorizontalAlignment::Right:
      adjusted_rect.position.x += draw_rect.size.x - size.x;
      break;
    default:
      break;
    }
  }
  return adjusted_rect;
}