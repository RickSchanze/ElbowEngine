//
// Created by Echo on 25-3-4.
//

#pragma once

#include "Core/Event/Event.h"
#include "IInteractable.h"
#include "Text.h"
#include "Widget.h"

namespace func::ui::widget {

DECLARE_MULTICAST_EVENT(ButtonEvent)

class CLASS() Button : public Widget, public IInteractable {
public:
  Button();
  void Rebuild(core::Rect2DI draw_rect) override;
  void Draw(platform::rhi::CommandBuffer &cmd) override;

  void OnMouseEnter() override;
  void OnMouseLeave() override;
  void OnMousePressed(const RespondMouses &button, Int32 x, Int32 y) override;
  void OnMouseReleased(const RespondMouses &button, Int32 x, Int32 y) override;
  core::Vector2I GetBoundingSize() override;

  ButtonEvent Evt_OnClicked;

private:
  PROPERTY()
  core::ObjectPtr<Text> text_;

  PROPERTY()
  Padding padding_;
};

} // namespace func::ui::widget
