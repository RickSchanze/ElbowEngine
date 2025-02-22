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

  void Draw(platform::rhi::CommandBuffer &cmd) override;
  void Rebuild(core::Rect2DI target_rect, core::Array<platform::rhi::Vertex_UI> &vertex_buffer,
               core::Array<UInt32> &index_buffer) override;

private:
  PROPERTY()
  core::ObjectPtr<Text> title_;

protected:
  UInt64 index_size_ = 0;
};
} // namespace func::ui::widget
