//
// Created by Echo on 25-1-30.
//

#pragma once
#include "Resource/Assets/Asset.h"

#include "Core/Math/MathTypes.h"
#include GEN_HEADER("Resource.Font.generated.h")

namespace resource {
class FontMeta;
}
namespace resource {
class Texture2D;
}
namespace resource {

enum class FontRenderMethod {
  // TODO: 实现SDF Font
  SDF,
  Bitmap,
};

struct GlyphInfo {
  Float uv_x_lt, uv_y_lt, uv_x_rb, uv_y_rb;
  UInt32 bearing_x, bearing_y; // 字符的偏移
  UInt32 width, height;
  UInt32 advance_x;
};

class CLASS() Font : public Asset {
  GENERATED_CLASS(Font)
public:
  ~Font();

  [[nodiscard]] AssetType GetAssetType() const override { return resource::AssetType::Font; }

  void PerformLoad() override;
  void PerformUnload() override;

  bool Load(const FontMeta &meta);

  [[nodiscard]] bool IsLoaded() const override;

  [[nodiscard]] Int16 GetFontSize() const { return font_size_; }
  /**
   * 设置字体大小, 这会生成新的FontAtlas
   */
  void SetFontSize(Int16 new_size);

  [[nodiscard]] Texture2D *GetFontAtlas() const { return font_atlas_; }

  [[nodiscard]] FontRenderMethod GetRenderMethod() const { return render_method_; }
  [[nodiscard]] core::StringView GetAssetPath() const { return path_; }

  [[nodiscard]] const GlyphInfo &GetGlyphInfo(UInt32 unicode) const { return glyphs_.at(unicode); }
  [[nodiscard]] bool HasGlyph(UInt32 unicode) const { return glyphs_.contains(unicode); }

  void RequestLoadGlyphs(const core::UnicodeString &str);

private:
  void RequestLoadGlyph(UInt32 code_point);
  Int32 font_size_ = 32;                                      // 静态字体大小
  FontRenderMethod render_method_ = FontRenderMethod::Bitmap; // 默认不使用SDF
  core::String path_;                                         // 资源路径
  Int32 font_atlas_width_ = 0;                                // 字体图集的宽度
  Int32 font_atlas_height_ = 0;                               // 字体图集的高度
  core::ObjectPtr<Texture2D> font_atlas_ = nullptr;           // 字体图集
  core::HashMap<UInt32, GlyphInfo> glyphs_;                   // 记录每个字符对应font atlas的信息

  struct FontHandle;
  FontHandle *dynamic_font_handle_ = nullptr; // 当字体是dynamic时有用
  core::Vector2u cursor_ = {0, 0};
};

} // namespace resource
