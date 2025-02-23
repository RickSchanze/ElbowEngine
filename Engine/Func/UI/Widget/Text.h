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
  Text &SetText(core::StringView text);
  Text &SetSpacing(Float space);
  Text &SetFont(const resource::Font *font);
  Text &SetFontSize(Float size);
  Text &SetFontMaterial(const resource::Material *mat);
  Text &SetColor(core::Color color);
  core::Rect2DI GetBoundingRect();
  core::StringView GetText() const { return text_; }

  /**
   * 计算可以容纳文字的矩形框大小
   * @return
   */
  core::Rect2DI GetFontRect();

  [[nodiscard]] core::Color GetFontColor() const { return font_color_; }

  void Draw(platform::rhi::CommandBuffer &cmd) override;

  void Rebuild(core::Rect2DI draw_rect, core::ArrayProxy<platform::rhi::Vertex_UI> &vertices,
               core::ArrayProxy<UInt32> &indices) override;

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
  core::Color font_color_ = core::Color::White();

  PROPERTY(Label = "字体材质")
  core::ObjectPtr<resource::Material> font_material_;
};

} // namespace func::ui::widget
