//
// Created by Echo on 25-2-22.
//

#pragma once
#include "Panel.h"

#include GEN_HEADER("Func.WindowPanel.generated.h")
#include "IInteractable.h"

namespace func::ui::widget {
class Text;
}
namespace func::ui::widget {
class CLASS() WindowPanel : public Panel, public  IInteractable {
  GENERATED_CLASS(WindowPanel)
public:
  WindowPanel();

  void SetTitle(const core::StringView &new_t);
  [[nodiscard]] core::StringView GetTitle() const;
  void Draw(platform::rhi::CommandBuffer &cmd) override;
  void Rebuild(core::Rect2DI draw_rect) override;
  void SetFocused(Bool f);
  [[nodiscard]] bool IsFocused() const { return focused_; }

  virtual void OnGetFocused();
  virtual void OnLoseFocused();

protected:
  PROPERTY()
  core::ObjectPtr<Text> title_;

  PROPERTY()
  Int32 title_height_ = 20;

  Bool expanded_ = true;

  UInt64 index_size_ = 0;

  Bool focused_ = false;
};
} // namespace func::ui::widget
