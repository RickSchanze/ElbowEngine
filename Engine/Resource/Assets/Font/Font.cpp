//
// Created by Echo on 25-1-30.
//

#include "Font.h"

#include "ft2build.h"
#include FT_FREETYPE_H
#include "Core/Math/Math.h"
#include "FontMeta.h"
#include "Platform/FileSystem/File.h"
#include "Platform/RHI/Image.h"
#include "Resource/AssetDataBase.h"
#include "Resource/Assets/Texture/Texture2D.h"
#include "Resource/Assets/Texture/Texture2DMeta.h"
#include "Resource/Logcat.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "Platform/RHI/GfxCommandHelper.h"

#include "Resource/Assets/Material/Material.h"
#include "Resource/Assets/Material/MaterialMeta.h"
#include "Resource/Assets/Shader/Shader.h"

#include GEN_HEADER("Resource.Font.generated.h")
using namespace resource;
using namespace core;
using namespace platform;
using namespace rhi;

GENERATED_SOURCE()

class FontLib : public Singleton<FontLib> {
public:
  FontLib() {
    Int32 errcode = FT_Init_FreeType(&library_);
    Assert::Require("Resource.Load.Font", errcode == 0, "初始化FreeType失败, 错误码={}.", errcode);
  }

  static Int32 Load(StringView path, FT_Face &face) { return FT_New_Face(GetByRef().library_, *path, 0, &face); }

  static void SetFontSize(FT_Face face, Int32 size) { FT_Set_Pixel_Sizes(face, size, size); }

  static Int32 LoadChar(FT_Face face, UInt32 unicode) {
    const Int32 errcode = FT_Load_Char(face, unicode, FT_LOAD_RENDER);
    return errcode;
  }

  static Int32 SetCharMapUnicode(FT_Face face) { return FT_Select_Charmap(face, FT_ENCODING_UNICODE); }

private:
  FT_Library library_ = nullptr;
};

Font *Font::GetDefaultFont() {
  return AssetDataBase::Load<Font>("Assets/Font/MapleMono.ttf");
}

Material *Font::GetDefaultFontMaterial() {
  auto mat_path = "Assets/Material/DefaultFont.mat";
  auto shader_path = "Assets/Shader/Text.slang";
  if (auto meta = AssetDataBase::QueryMeta<MaterialMeta>(String::Format("path = '{}'", mat_path))) {
    return AssetDataBase::Load<Material>(meta.GetValue().path);
  } else {
    auto *text_shader = AssetDataBase::Load<Shader>(shader_path);
    auto *font_material = ObjectManager::CreateNewObject<Material>();
    font_material->SetShader(text_shader);
    AssetDataBase::CreateAsset(font_material, mat_path);
    return font_material;
  }
}

Font::~Font() {
  font_atlas_ = nullptr;
  Delete(dynamic_font_handle_);
  dynamic_font_handle_ = nullptr;
}

void Font::PerformLoad() {
  auto op_meta = AssetDataBase::QueryMeta<FontMeta>(GetHandle());
  if (!op_meta) {
    LOGGER.Error(logcat::Resource, "加载失败, handle = {}", GetHandle());
    return;
  }
  auto &meta = *op_meta;
  Load(meta);
}

void Font::PerformUnload() { font_atlas_ = nullptr; }

struct Font::FontHandle {
  FT_Face face = nullptr;

  [[nodiscard]] bool IsLoaded() const { return face != nullptr; }

  FontHandle(core::StringView path, Int32 size) {
    Int32 errcode = FontLib::Load(path, face);
    if (errcode != 0) {
      LOGGER.Error("Resource.Load.Font", "加载字体{}失败, 错误码={}", path, errcode);
    } else {
      FontLib::SetFontSize(face, size);
    }
  }

  ~FontHandle() { FT_Done_Face(face); }
};

bool Font::Load(const FontMeta &meta) {
  path_ = meta.path;
  font_atlas_width_ = meta.font_atlas_width;
  font_atlas_height_ = meta.font_atlas_height;
  font_size_ = meta.font_size;
  if (Math::WillMultiplyOverflow(font_atlas_width_, font_atlas_height_)) {
    LOGGER.Error("Resource.Load.Font", "FontAtlas设置得太大啦!");
    return false;
  }
  Texture2DMeta atlas_meta;
  atlas_meta.width = font_atlas_width_;
  atlas_meta.height = font_atlas_height_;
  atlas_meta.dynamic = true;
  atlas_meta.format = Format::R8_SRGB;
  Texture2D *font_atlas = ObjectManager::CreateNewObjectAsync<Texture2D>()->GetValue().GetValue() | First;
  font_atlas->SetName(String::Format("Font Atlas for {}", path_));
  font_atlas->Load(atlas_meta);
  font_atlas_ = font_atlas;
  dynamic_font_handle_ = New<FontHandle>(path_, font_size_);
  return IsLoaded();
}

void Font::SetFontSize(Int16 new_size) {
  if (new_size != font_size_) {
    font_size_ = new_size;
    SetDirty(true);
  }
}

void Font::RequestLoadGlyphs(const UnicodeString &str) {
  if (!IsLoaded()) {
    LOGGER.Error("Resource.Font", "字体尚未加载");
    return;
  }
  for (UInt64 i = 0; i < str.Size(); ++i) {
    RequestLoadGlyph(str.At(i));
  }
}

void Font::RequestLoadGlyph(UInt32 code_point) {
  if (glyphs_.contains(code_point))
    return;
  UInt32 errcode = FontLib::LoadChar(dynamic_font_handle_->face, code_point);
  if (errcode != 0) {
    LOGGER.Warn("Resource.Load.Font", "加载字符{}失败(<-这是Unicode代码), 错误码={}.", code_point, errcode);
    return;
  }
  FT_GlyphSlot slot = dynamic_font_handle_->face->glyph;
  FT_Bitmap &bitmap = slot->bitmap;
  if (cursor_.x + bitmap.width >= font_atlas_width_) {
    cursor_.x = 0;
    cursor_.y += bitmap.rows;
  }
  if (cursor_.y + bitmap.rows >= font_atlas_height_) {
    LOGGER.Error("Resource.Load.Font", "字体{}的字体图集不够大.", path_);
    return;
  }
  Array<UInt8> data;
  data.reserve(100);
  for (Int32 y = 0; y < bitmap.rows; y++) {
    for (Int32 x = 0; x < bitmap.width; x++) {
      data.push_back(bitmap.buffer[x + y * bitmap.pitch]);
    }
  }
  GfxCommandHelper::CopyDataToImage2D(data.data(), font_atlas_->GetNativeImage(), data.size(),
                                      {cursor_.x, cursor_.y, 0}, {bitmap.width, bitmap.rows, 1});
  // 保存此字符的信息
  GlyphInfo info{};
  // 半像素校正UV
  info.uv_x_lt = static_cast<Float>(cursor_.x + 0.5) / static_cast<Float>(font_atlas_width_);
  info.uv_y_lt = static_cast<Float>(cursor_.y + 0.5) / static_cast<Float>(font_atlas_height_);
  info.uv_x_rb = static_cast<Float>(cursor_.x + bitmap.width - 1) / static_cast<Float>(font_atlas_width_);
  info.uv_y_rb = static_cast<Float>(cursor_.y + bitmap.rows - 1) / static_cast<Float>(font_atlas_height_);
  info.advance_x = slot->advance.x >> 6;
  info.bearing_x = slot->metrics.horiBearingX >> 6;
  info.bearing_y = slot->metrics.horiBearingY >> 6;
  info.width = bitmap.width;
  info.height = bitmap.rows;
  cursor_.x += bitmap.width;
  glyphs_[code_point] = info;
}

bool Font::IsLoaded() const {
  Texture2D *atlas = font_atlas_;
  return atlas && atlas->IsLoaded() && dynamic_font_handle_ && dynamic_font_handle_->IsLoaded();
}
