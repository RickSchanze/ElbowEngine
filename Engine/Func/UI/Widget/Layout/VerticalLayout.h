//
// Created by Echo on 25-3-3.
//

#pragma once
#include "Core/Object/ObjectPtr.h"
#include "Func/UI/Widget/Widget.h"

#include GEN_HEADER("Func.VerticalLayout.generated.h")

namespace func::ui::widget {

class CLASS() VerticalLayout : public Widget {
  GENERATED_CLASS(VerticalLayout)
public:
  VerticalLayout();
  void Rebuild(core::Rect2DI draw_rect) override;
  void Draw(platform::rhi::CommandBuffer &cmd) override;

  VerticalLayout *AddChild(Widget *w);

protected:
  PROPERTY()
  core::Array<core::ObjectPtr<Widget>> children_;
  core::Array<Widget *> cached_children_;

  PROPERTY()
  VerticalAlignment vertical_alignment_ = VerticalAlignment::Top;

  PROPERTY()
  Int32 spacing_;
};
} // namespace func::ui::widget
