//
// Created by Echo on 25-2-22.
//

#pragma once
#include "Panel.h"

#include GEN_HEADER("Func.WindowPanel.generated.h")

namespace func::ui::widget {
class Text;
}
namespace func::ui::widget {
class CLASS() WindowPanel : public Panel {
  GENERATED_CLASS(WindowPanel)
public:
  WindowPanel();

  void SetTitle(const core::StringView &new_t);
  core::StringView GetTitle() const;
  void Draw(platform::rhi::CommandBuffer &cmd) override;
  void Rebuild(core::Rect2DI draw_rect) override;

protected:
  PROPERTY()
  core::ObjectPtr<Text> title_;

  PROPERTY()
  Int32 title_height_ = 20;

  Bool expanded_ = true;

  UInt64 index_size_ = 0;
};
} // namespace func::ui::widget
