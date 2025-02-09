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
  Text &SetFont(resource::Font *font);
  Text &SetFontSize(Float size);
  Text &SetFontMaterial(resource::Material *mat);
  Text &SetColor(core::Color color);
  core::Rect2D GetBoundingRect() override;

  /**
   * 计算可以容纳文字的矩形框大小
   * @return
   */
  core::Rect2D GetFontRect();

  [[nodiscard]] core::Color GetFontColor() const { return font_color_; }

  void Rebuild(core::Rect2D target_rect, core::Array<platform::rhi::Vertex_UI> &vertex_buffer,
               core::Array<UInt32> &index_buffer) override;

  void Draw(platform::rhi::CommandBuffer &cmd) override;

private:
  PROPERTY(Label = "文本")
  core::String text_;

  PROPERTY(Label = "字符间距")
  Float spacing_ = 0;

  PROPERTY(Label = "大小")
  Float size_ = 32;

  PROPERTY(Label = "基线位置")
  Float base_line_ = 0;

  PROPERTY(Label = "字体")
  core::ObjectPtr<resource::Font> font_;

  PROPERTY(Label = "字体颜色")
  core::Color font_color_ = core::Color::White();

  PROPERTY(Label = "字体材质")
  core::ObjectPtr<resource::Material> font_material_;

  UInt64 index_offset_ = 0;
  UInt64 index_range_ = 0;
};

} // namespace func::ui::widget
