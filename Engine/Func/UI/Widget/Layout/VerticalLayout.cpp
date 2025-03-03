//
// Created by Echo on 25-3-3.
//

#include "VerticalLayout.h"

#include "Core/Object/ObjectRegistry.h"

#include GEN_HEADER("Func.VerticalLayout.generated.h")

GENERATED_SOURCE()

using namespace func;
using namespace ui;
using namespace widget;

void VerticalLayout::Rebuild(core::Rect2DI draw_rect) {
  Int32 current_y = draw_rect.size.y;
  for (auto &child : cached_children_) {
    Widget *w = child;
    if (!w) {
      continue;
    }
    core::Vector2I size = w->GetBoundingSize();
    core::Rect2DI child_draw_rect{};
    child_draw_rect.position.x = draw_rect.position.x;
    child_draw_rect.position.y = current_y - size.y;
    child_draw_rect.size = size;
    w->Rebuild(child_draw_rect);
    current_y -= size.y;
    if (current_y < 0) {
      // TODO: 处理小于0的情况
      break;
    }
  }
}

void VerticalLayout::Draw(platform::rhi::CommandBuffer &cmd) {
  for (auto &child : cached_children_) {
    if (!child) {
      continue;
    }
    child->Draw(cmd);
  }
}

VerticalLayout *VerticalLayout::AddChild(Widget *w) {
  if (w) {
    children_.push_back(w);
    cached_children_.push_back(w);
  }
  return this;
}

void VerticalLayout::OnSetDirty() {
  for (auto w : cached_children_) {
    w->SetDirty();
  }
}
