//
// Created by Echo on 25-2-22.
//

#pragma once
#include "Panel.h"
#include "IInteractable.h"

#include GEN_HEADER("Func.WindowPanel.generated.h")


namespace func::ui::widget {
class Text;
}
namespace func::ui::widget {
class CLASS() WindowPanel : public Panel, public IInteractable {
  GENERATED_CLASS(WindowPanel)
public:
  WindowPanel();

  WindowPanel& SetTitle(const core::StringView &new_t);
  WindowPanel& SetSlot(Widget *w);

  [[nodiscard]] core::StringView GetTitle() const;
  void Draw(platform::rhi::CommandBuffer &cmd) override;
  void Rebuild(core::Rect2DI draw_rect) override;
  void SetFocused(Bool f);
  [[nodiscard]] bool IsFocused() const { return focused_; }

  virtual void OnGetFocused();
  virtual void OnLoseFocused();
  void OnSetDirty() override;
  void OnMousePressed(const RespondMouses &button, Int32 x, Int32 y) override;
  void OnMouseReleased(const RespondMouses &button, Int32 x, Int32 y) override;
  void OnMouseMove(Float x, Float y) override;
  void OnMouseLeave() override;
  void OnSetPosition(const core::Vector2I &old_pos, const core::Vector2I &new_pos) override;

protected:
  [[nodiscard]] core::Rect2DI GetTitleRect() const;

  PROPERTY()
  core::ObjectPtr<Text> title_;

  PROPERTY()
  Int32 title_height_ = 30;

  PROPERTY()
  core::ObjectPtr<Widget> slot_;

  Bool expanded_ = true;

  UInt64 index_size_ = 0;

  Bool focused_ = false;

  Bool moving_ = false;
};
} // namespace func::ui::widget
