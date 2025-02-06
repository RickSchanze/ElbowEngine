//
// Created by Echo on 25-1-30.
//

#include "Font.h"

#include "ft2build.h"
#include FT_FREETYPE_H
#include "Core/Math/Math.h"
#include "FontMeta.h"
#include "Platform/FileSystem/File.h"
#include "Platform/FileSystem/Path.h"
#include "Platform/RHI/Image.h"
#include "Resource/AssetDataBase.h"
#include "Resource/Assets/Texture/Texture2D.h"
#include "Resource/Assets/Texture/Texture2DMeta.h"
#include "Resource/Logcat.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

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

  static Int32 Load(core::StringView path, FT_Face &face) { return FT_New_Face(GetByRef().library_, *path, 0, &face); }

  static void SetFontSize(FT_Face face, Int32 size) { FT_Set_Pixel_Sizes(face, size, size); }

  static Int32 LoadChar(FT_Face face, UInt32 unicode) {
    const Int32 errcode = FT_Load_Char(face, unicode, FT_LOAD_RENDER);
    return errcode;
  }

  static Int32 SetCharMapUnicode(FT_Face face) { return FT_Select_Charmap(face, FT_ENCODING_UNICODE); }

private:
  FT_Library library_;
};

static bool LoadFont(StringView path, Int32 font_size, StringView charset_path, UInt8 *target_buffer, UInt64 atlas_w,
                     UInt64 atlas_h, HashMap<UInt32, GlyphInfo> &glyphs) {
  FT_Face face;
  Int32 errcode = FontLib::Load(path, face);
  if (errcode != 0) {
    LOGGER.Error("Resource.Load.Font", "加载字体{}失败, 错误码={}.", path, errcode);
    FT_Done_Face(face);
    return false;
  }
  font_size = font_size - 1;
  FontLib::SetFontSize(face, font_size);
  auto op_charset = File::ReadAllText(charset_path);
  if (!op_charset) {
    LOGGER.Error("Resource.Load.Font", "加载字符集{}失败.", charset_path);
    FT_Done_Face(face);
    return false;
  }
  auto &charset = *op_charset;
  if (charset.IsEmpty()) {
    LOGGER.Warn("Resource.Load.Font", "字符集为空, 是否忘记往里填充字符?");
  }

  Int32 pen_x = 0, pen_y = 0; // 当前写入x, y左边
  UInt32 row_height = 0;
  // TODO: Rect packing 更高效利用空间
  const UnicodeString unicode_charset = charset.AsUnicode();
  const UInt64 unicode_size = unicode_charset.Size();
  for (Int32 i = 0; i < unicode_size; i++) {
    UInt32 unicode = unicode_charset.At(i);
    if (glyphs.contains(unicode)) {
      continue;
    }
    if (Int32 char_errcode = FontLib::LoadChar(face, unicode); char_errcode != 0) {
      LOGGER.Warn("Resource.Load.Font", "加载字符{}失败(<-这是Unicode代码), 错误码={}.", unicode, char_errcode);
      continue;
    }
    const FT_GlyphSlot glyph_slot = face->glyph;
    const FT_Bitmap &bitmap = glyph_slot->bitmap;
    if (pen_x + bitmap.width >= atlas_w) { // 这一行放不下了
      pen_x = 0;
      pen_y += row_height + 1; // 转下一行
      row_height = 0;
    }
    if (pen_y + bitmap.rows >= atlas_h) {
      LOGGER.Warn("Resource.Load.Font", "Font Atlas已满! {}/{}", i, unicode_size);
      break;
    }
    if (target_buffer != nullptr) {
      // 拷贝数据
      for (Int32 y = 0; y < bitmap.rows; y++) {
        for (Int32 x = 0; x < bitmap.width; x++) {
          Int32 target_index = (pen_y + y) * atlas_w + pen_x + x;
          target_buffer[target_index] = bitmap.buffer[y * bitmap.pitch + x];
        }
      }
    }
    // 保存此字符的信息
    GlyphInfo info{};
    info.uv_x_lt = static_cast<Float>(pen_x) / atlas_w;
    info.uv_y_lt = static_cast<Float>(pen_y) / atlas_h;
    info.uv_x_rb = static_cast<Float>(pen_x + bitmap.width) / atlas_w;
    info.uv_y_rb = static_cast<Float>(pen_y + bitmap.rows) / atlas_h;
    info.advance_x = glyph_slot->advance.x >> 6;
    info.bearing_x = glyph_slot->metrics.horiBearingX >> 6;
    info.bearing_y = glyph_slot->metrics.horiBearingY >> 6;
    info.width = bitmap.width;
    info.height = bitmap.rows;
    glyphs[unicode] = info;
    pen_x += bitmap.width + 1;
    row_height = std::max(row_height, bitmap.rows);
  }
  FT_Done_Face(face);
  return true;
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

static Texture2D *LoadFontAtlasTexture(StringView atlas_path) {
  Texture2D *tex = *ObjectManager::CreateNewObject<Texture2D>()->GetValue() | First;
  if (tex) {
    Texture2DMeta new_meta{};
    new_meta.format = Format::R8_UNorm;
    new_meta.path = atlas_path;
    tex->Load(new_meta);
  }
  return tex;
}

bool Font::Load(const FontMeta &meta) {
  path_ = meta.path;
  font_atlas_width_ = meta.font_atlas_width;
  font_atlas_height_ = meta.font_atlas_height;
  font_size_ = meta.font_size;
  if (Math::WillMultiplyOverflow(font_atlas_width_, font_atlas_height_)) {
    LOGGER.Error("Resource.Load.Font", "FontAtlas设置得太大啦!");
    return false;
  }
  Int32 font_atlas_size = font_atlas_width_ * font_atlas_height_;
  StringView temp_font_atlas_name = Path::GetFileNameWithoutExt(GetAssetPath());
  String temp_font_atlas_path = Path::Combine("Intermediate", String::Format("FontAtlas_{}.png", temp_font_atlas_name));
  if (File::IsExist(temp_font_atlas_path)) {
    font_atlas_ = LoadFontAtlasTexture(temp_font_atlas_path);
  }
  UInt8 *buffer = nullptr;
  if (!font_atlas_) {
    buffer = static_cast<UInt8 *>(Malloc(font_atlas_size));
    memset(buffer, 0, font_atlas_size);
  }
  if (LoadFont(path_, font_size_, meta.font_charset_file, buffer, font_atlas_width_, font_atlas_height_,
               this->glyphs_) &&
      !font_atlas_) {
    // 写入字体贴图中, 然后通过Texture2D的接口加载这张图片
    Int32 write_errcode =
        stbi_write_png(*temp_font_atlas_path, font_atlas_width_, font_atlas_height_, 1, buffer, font_atlas_width_);
    if (write_errcode != 1) {
      LOGGER.Error("Resource.Load.Font", "写入FontAtlas失败, 错误码={}.", write_errcode);
    } else {
      font_atlas_ = LoadFontAtlasTexture(temp_font_atlas_path);
    }
  }
  if (buffer != nullptr) {
    Free(buffer);
  }
  return font_atlas_;
}

void Font::SetFontSize(Int16 new_size) {
  if (new_size != font_size_) {
    font_size_ = new_size;
    SetDirty(true);
  }
}