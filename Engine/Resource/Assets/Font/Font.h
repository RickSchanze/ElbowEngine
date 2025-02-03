//
// Created by Echo on 25-1-30.
//

#pragma once
#include "Resource/Assets/Asset.h"

#include GEN_HEADER("Resource.Font.generated.h")

namespace resource {
class FontMeta;
}
namespace resource {
class Texture2D;
}
namespace resource {

enum class FontRenderMethod {
  SDF,
  Texture,
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
  [[nodiscard]] AssetType GetAssetType() const override { return resource::AssetType::Font; }

  void PerformLoad() override;
  void PerformUnload() override;

  bool Load(const FontMeta &meta);

  [[nodiscard]] Int16 GetFontSize() const { return font_size_; }
  /**
   * 设置字体大小, 这会生成新的FontAtlas
   */
  void SetFontSize(Int16 new_size);

  [[nodiscard]] Texture2D* GetFontAtlas() const { return font_atlas_; }

  [[nodiscard]] FontRenderMethod GetRenderMethod() const { return render_method_; }
  [[nodiscard]] core::StringView GetAssetPath() const { return path_; }

  [[nodiscard]] const GlyphInfo &GetGlyphInfo(UInt32 unicode) const { return glyphs_.at(unicode); }
  [[nodiscard]] bool HasGlyph(UInt32 unicode) const { return glyphs_.contains(unicode); }

private:
  Int32 font_size_ = 16;                                       // 静态字体大小
  FontRenderMethod render_method_ = FontRenderMethod::Texture; // 默认不使用SDF
  core::String path_;                                          // 资源路径
  Int32 font_atlas_width_ = 0;                                 // 字体图集的宽度
  Int32 font_atlas_height_ = 0;                                // 字体图集的高度
  core::ObjectPtr<Texture2D> font_atlas_ = nullptr;            // 字体图集
  // 字符集文件路径, 这个文件用于表明该字体需要加载什么字符
  core::String charset_file_;
  core::HashMap<UInt32, GlyphInfo> glyphs_; // 记录每个字符对应font atlas的信息
};
} // namespace resource
