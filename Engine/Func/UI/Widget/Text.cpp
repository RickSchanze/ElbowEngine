//
// Created by Echo on 25-2-1.
//

#include "Text.h"

#include "Core/Math/Math.h"
#include "Func/Render/Misc.h"
#include "Platform/RHI/CommandBuffer.h"
#include "Platform/RHI/Commands.h"
#include "Platform/RHI/VertexLayout.h"
#include "Resource/Assets/Font/Font.h"
#include "Resource/Assets/Material/Material.h"

using namespace func;
using namespace ui;
using namespace widget;
using namespace core;
using namespace resource;
using namespace platform;
using namespace rhi;

Text &Text::SetText(StringView text) {
  if (text == text_)
    return *this;
  text_ = text;
  SetDirty();
  return *this;
}

Text &Text::SetSpacing(Float space) {
  if (Math::ApproximatelyEqual(space, spacing_)) {
    return *this;
  }
  spacing_ = space;
  SetDirty();
  return *this;
}

Text &Text::SetFont(const Font *font) {
  if (font == nullptr)
    return *this;
  if (font_ == font)
    return *this;
  font_ = font;
  if (font_material_) {
    font_material_->SetTexture2D("atlas", font_->GetFontAtlas());
  }
  SetDirty();
  return *this;
}

Text &Text::SetFontSize(Float size) {
  if (Math::ApproximatelyEqual(size, size_))
    return *this;
  size_ = size;
  SetDirty();
  return *this;
}

Text &Text::SetFontMaterial(const Material *mat) {
  if (mat == nullptr)
    return *this;
  if (font_material_ == mat)
    return *this;
  font_material_ = mat;
  if (const Font *font = font_) {
    font_material_->SetTexture2D("atlas", font->GetFontAtlas());
  }
  SetDirty();
  return *this;
}

Text &Text::SetColor(core::Color color) {
  if (font_color_ != color) {
    font_color_ = color;
    SetDirty();
  }
  return *this;
}

Rect2DI Text::GetBoundingRect() { return GetFontRect(); }

Rect2DI Text::GetFontRect() {
  UnicodeString str = text_.ToUnicodeString();
  UInt64 size = str.Size();
  Font *font = font_;
  if (font == nullptr) {
    LOGGER.Error("Func.UI.Text", "字体未设置");
    return Rect2DI{};
  }
  UInt32 bearing_height_top = 0;
  UInt32 bearing_height_bottom = 0;
  UInt32 width = 0;
  for (UInt64 i = 0; i < size; ++i) {
    UInt32 unicode = str.At(i);
    if (!font->HasGlyph(unicode)) {
      LOGGER.Warn("Func.UI.Text", "字体{}不存在字符{}", font->GetAssetPath(), unicode);
      continue;
    }
    auto &glyph = font->GetGlyphInfo(unicode);
    width += glyph.advance_x;
    bearing_height_top = std::max(bearing_height_top, glyph.bearing_y + base_line_);
    bearing_height_bottom = std::max(bearing_height_bottom, glyph.height - glyph.bearing_y + base_line_);
  }
  Rect2DI rect{};
  rect.position.x = 0;
  rect.position.y = 0;
  auto padding = GetPadding();
  rect.size.x = width + padding.x + padding.z + (size - 1) * spacing_;
  rect.size.y = bearing_height_top + bearing_height_bottom + padding.y + padding.w;
  return rect;
}

void Text::Draw(CommandBuffer &cmd) {
  Material *mat = font_material_;
  if (mat == nullptr) {
    return;
  }
  BindMaterial(cmd, mat);
  cmd.Enqueue<Cmd_DrawIndexed>(index_size_, 1, index_offset_);
}

void Text::Rebuild(Rect2DI draw_rect, ArrayProxy<Vertex_UI> &vertices, ArrayProxy<UInt32> &indices) {
  if (!IsDirty()) {
    return;
  }
  // 左下角
  Vector2I bl = BottomLeft(draw_rect);
  Vector2I rt = TopRight(draw_rect);
  UnicodeString str = text_.ToUnicodeString();
  UInt64 size = str.Size();
  Vector4I padding = GetPadding();
  index_offset_ = indices.Size();
  UInt32 cur_pos_x = bl.x + padding.x;
  UInt32 cur_pos_y = bl.y + base_line_ + padding.w;

  Font *font = font_;
  if (font == nullptr) {
    LOGGER.Error("Func.UI.Text", "字体未设置");
    return;
  }
  font->RequestLoadGlyphs(str);
  Float font_scale = static_cast<Float>(size_) / font->GetFontSize();
  auto spacing = spacing_;
  for (UInt64 i = 0; i < size; ++i) {
    if (cur_pos_x >= rt.x || cur_pos_y >= rt.y) {
      break;
    }
    UInt32 unicode = str.At(i);
    auto &glyph = font_->GetGlyphInfo(unicode);

    Vertex_UI left_top{};
    left_top.position.x = cur_pos_x + glyph.bearing_x * font_scale;
    left_top.position.y = cur_pos_y + glyph.bearing_y * font_scale;
    left_top.uv.x = glyph.uv_x_lt;
    left_top.uv.y = glyph.uv_y_lt;
    left_top.color = font_color_;

    Vertex_UI left_bottom{};
    left_bottom.position.x = cur_pos_x + glyph.bearing_x * font_scale;
    left_bottom.position.y = cur_pos_y - (glyph.height - glyph.bearing_y) * font_scale;
    left_bottom.uv.x = glyph.uv_x_lt;
    left_bottom.uv.y = glyph.uv_y_rb;
    left_bottom.color = font_color_;

    Vertex_UI right_top{};
    right_top.position.x = cur_pos_x + (glyph.bearing_x + glyph.width) * font_scale;
    right_top.position.y = cur_pos_y + glyph.bearing_y * font_scale;
    right_top.uv.x = glyph.uv_x_rb;
    right_top.uv.y = glyph.uv_y_lt;
    right_top.color = font_color_;

    Vertex_UI right_bottom{};
    right_bottom.position.x = cur_pos_x + (glyph.bearing_x + glyph.width) * font_scale;
    right_bottom.position.y = cur_pos_y - (glyph.height - glyph.bearing_y) * font_scale;
    right_bottom.uv.x = glyph.uv_x_rb;
    right_bottom.uv.y = glyph.uv_y_rb;
    right_bottom.color = font_color_;

    vertices.Add(left_top);
    vertices.Add(left_bottom);
    vertices.Add(right_top);
    vertices.Add(right_bottom);

    UInt64 index_size = vertices.Size();
    indices.Add(index_size - 3);
    indices.Add(index_size - 1);
    indices.Add(index_size - 2);
    indices.Add(index_size - 3);
    indices.Add(index_size - 2);
    indices.Add(index_size - 4);
    index_size_ += 6;

    cur_pos_x += ((glyph.bearing_x + glyph.width) * font_scale + spacing);
  }
  SetDirty(false);
}