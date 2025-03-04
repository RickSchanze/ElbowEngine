//
// Created by Echo on 25-2-1.
//

#pragma once
#include "Core/Object/ObjectPtr.h"
#include "Widget.h"

namespace resource {
class Material;
}
namespace resource {
class Font;
}
namespace func::ui::widget {

class CLASS() Text : public Widget {
public:
  Text();
  Text *SetText(core::StringView text);
  Text &SetSpacing(Float space);
  Text &SetFont(const resource::Font *font);
  Text *SetTextSize(Float size);
  Text &SetFontMaterial(const resource::Material *mat);
  Text &SetColor(core::Color color);
  [[nodiscard]] core::StringView GetText() const { return text_; }
  [[nodiscard]] core::Color GetFontColor() const { return font_color_; }

  core::Vector2I GetBoundingSize() override;
  void Draw(platform::rhi::CommandBuffer &cmd) override;
  void Rebuild(core::Rect2DI draw_rect) override;

private:
  PROPERTY(Label = "文本")
  core::String text_;

  PROPERTY(Label = "字符间距")
  Int32 spacing_ = 0;

  PROPERTY(Label = "大小")
  Int32 size_ = 32;

  PROPERTY(Label = "基线位置")
  Int32 base_line_ = 0;

  PROPERTY(Label = "字体")
  core::ObjectPtr<resource::Font> font_;

  PROPERTY(Label = "字体颜色")
  core::Color font_color_;

  PROPERTY(Label = "字体材质")
  core::ObjectPtr<resource::Material> font_material_;

  PROPERTY(Label = "竖直对齐")
  VerticalAlignment vertical_alignment_ = VerticalAlignment::Center;
};

} // namespace func::ui::widget
