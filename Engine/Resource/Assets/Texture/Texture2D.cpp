//
// Created by Echo on 25-1-15.
//

#include "Texture2D.h"

#include "Platform/FileSystem/Path.h"
#include "Resource/AssetDataBase.h"
#include "Resource/Logcat.h"
#include "Texture2DMeta.h"

#define STB_IMAGE_IMPLEMENTATION
#include "Platform/RHI/GfxCommandHelper.h"
#include "Platform/RHI/GfxContext.h"
#include "Platform/RHI/Image.h"
#include "Platform/RHI/ImageView.h"
#include "stb_image.h"

#include "Core/Base/Ranges.h"
#include "Func/Algo/RectPacking.h"
#include <stb_image_write.h>

#include "Platform/FileSystem/File.h"
#include GEN_HEADER("Resource.Texture2D.generated.h")

#include <fstream>

GENERATED_SOURCE()

static stbi_uc *LoadImageStb(core::StringView path, int *width, int *height, int *channels) {
#ifdef PLATFORM_WINDOWS
  auto str = path.ToWideString();
  FILE *f = _wfopen(str.c_str(), L"rb");
  if (!f)
    return nullptr;
  stbi_uc *pixels = stbi_load_from_file(f, width, height, channels, 4);
  fclose(f);
  return pixels;
#else
  return nullptr;
#endif
}

using namespace core;
using namespace resource;
using namespace platform;
using namespace rhi;

bool SpriteRange::IsValid() const { return range.size.x != 0 && range.size.y != 0; }

void Texture2D::PerformLoad() {
  PROFILE_SCOPE_AUTO;
  auto meta_op = AssetDataBase::QueryMeta<Texture2DMeta>(GetHandle());
  if (!meta_op) {
    LOGGER.Error(logcat::Resource, "加载失败: handle为{}的Texture2D不在资产数据库", GetHandle());
    return;
  }
  Load(*meta_op);
}

void Texture2D::PerformUnload() {
  native_image_view_ = nullptr;
  native_image_ = nullptr;
}

void Texture2D::Load(const Texture2DMeta &meta) {
  PROFILE_SCOPE_AUTO;
  if (!meta.dynamic) {
    StringView path = meta.path;
    if (!path.EndsWith(".png")) {
      LOGGER.Error(logcat::Resource, "加载失败: Texture2D必须以.png结尾: {}", path);
      return;
    }
    if (!Path::IsExist(path)) {
      LOGGER.Error(logcat::Resource, "加载失败: 路径为{}的Texture2D文件不存在", path);
      return;
    }
    Int32 width = 0, height = 0, channels = 0;
    stbi_uc *pixels = stbi_load(*path, &width, &height, &channels, STBI_rgb_alpha);
    if (!pixels) {
      LOGGER.Error(logcat::Resource, "加载失败: 路径为{}的Texture2D文件无法加载", path);
      stbi_image_free(pixels);
      return;
    }
    Format format = Format::R8G8B8A8_UNorm;
    ImageDesc desc{static_cast<size_t>(width), static_cast<size_t>(height), IUB_TransferDst | IUB_ShaderRead, format,
                   ImageDimension::D2};
    String debug_name = String::Format("Texture2D_{}", path);
    native_image_ = GetGfxContextRef().CreateImage(desc, debug_name);
    ImageViewDesc view_desc{native_image_.get()};
    debug_name = String::Format("Texture2DView_{}", path);
    native_image_view_ = GetGfxContextRef().CreateImageView(view_desc, debug_name);
    GfxCommandHelper::CopyDataToImage2D(pixels, native_image_.get(), width * height * channels);
    stbi_image_free(pixels);
    asset_path_ = meta.path;
    SetSpriteRangeString(meta.sprites_string);
  } else {
    if (meta.width == 0 || meta.height == 0) {
      LOGGER.Error(logcat::Resource, "加载失败: Texture2D的宽度和高度必须大于0");
      return;
    }
    Format format = meta.format;
    // 这里设为TransferSrc是因为很可能是要作为之后保存的图像创建的
    ImageDesc desc{meta.width, meta.height, IUB_TransferDst | IUB_ShaderRead | IUB_TransferSrc, format,
                   ImageDimension::D2};
    String debug_name = String::Format("Texture2D_{}", name_);
    native_image_ = GetGfxContextRef().CreateImage(desc, debug_name);
    ImageViewDesc view_desc{native_image_.get()};
    debug_name = String::Format("Texture2DView_{}", name_);
    native_image_view_ = GetGfxContextRef().CreateImageView(view_desc, debug_name);
    asset_path_ = meta.path;
    SetSpriteRangeString(meta.sprites_string);
  }
}

UInt32 Texture2D::GetWidth() const {
  PROFILE_SCOPE_AUTO;
  if (!native_image_)
    return 0;
  return native_image_->GetWidth();
}

UInt32 Texture2D::GetHeight() const {
  PROFILE_SCOPE_AUTO;
  if (!native_image_)
    return 0;
  return native_image_->GetHeight();
}

UInt32 Texture2D::GetNumChannels() const { return native_image_->GetNumChannels(); }

Format Texture2D::GetFormat() const {
  PROFILE_SCOPE_AUTO;
  if (!native_image_)
    return Format::Count;
  return native_image_->GetFormat();
}

Texture2D *Texture2D::GetDefault() {
  PROFILE_SCOPE_AUTO;
  auto rtn = AssetDataBase::Load<Texture2D>("Assets/Texture/Default.png");
  Assert::Require(logcat::Resource, rtn != nullptr, "Default.png不存在");
  return rtn;
}

Rect2D Texture2D::GetUVRect(const SpriteRange &sprite_range) const {
  PROFILE_SCOPE_AUTO;
  Rect2D uv_rect;
  uv_rect.position.x = static_cast<Float>(sprite_range.range.position.x) / static_cast<Float>(GetWidth());
  uv_rect.position.y = static_cast<Float>(sprite_range.range.position.y) / static_cast<Float>(GetHeight());
  uv_rect.size.x = static_cast<Float>(sprite_range.range.size.x) / static_cast<Float>(GetWidth());
  uv_rect.size.y = static_cast<Float>(sprite_range.range.size.y) / static_cast<Float>(GetHeight());
  return uv_rect;
}

SpriteRange Texture2D::GetSpriteRange(UInt64 id) const {
  PROFILE_SCOPE_AUTO;
  for (const auto &sprite_range : sprite_ranges_) {
    if (sprite_range.id == id) {
      return sprite_range;
    }
  }
  return {};
}

SpriteRange Texture2D::GetSpriteRange(StringView name) const { return GetSpriteRange(name.GetHashCode()); }

void Texture2D::SetAssetPath(core::StringView new_path) {
  if (new_path == asset_path_)
    return;
  Assert::Require("Resource.Texture2D", asset_path_.IsEmpty(), "SetAssetPath: 此纹理已有路径: {}", asset_path_);
  asset_path_ = new_path;
}

bool Texture2D::AppendSprite(UInt64 id, const char *data, Rect2DI target_rect) {
  PROFILE_SCOPE_AUTO;
  if (target_rect.position.x + target_rect.size.x > GetWidth() ||
      target_rect.position.y + target_rect.size.y > GetHeight()) {
    LOGGER.Error("Resource.Texture2D", "target_rect超出了Texture2D的尺寸");
    return false;
  }
  if (range::AnyOf(sprite_ranges_, [id](const SpriteRange &sprite_range) { return sprite_range.id == id; })) {
    LOGGER.Error("Resource.Texture2D", "id为{}的SpriteRange已经存在", id);
    return false;
  }
  if (!data) {
    return false;
  }
  // 拷贝数据到图像
  auto num_channels = GetNumChannels();
  GfxCommandHelper::CopyDataToImage2D(data, native_image_.get(), target_rect.Area() * num_channels,
                                      {target_rect.position.x, target_rect.position.y, 0},
                                      {target_rect.size.x, target_rect.size.y, 1});
  SpriteRange new_sprite_range{};
  new_sprite_range.id = id;
  new_sprite_range.range = target_rect;
  sprite_ranges_.push_back(new_sprite_range);
  return true;
}

bool Texture2D::AppendSprite(UInt64 id, char *data, UInt32 width, UInt32 height) {
  PROFILE_SCOPE_AUTO;
  Vector2I bound;
  UInt32 tex_w = GetWidth();
  UInt32 tex_h = GetHeight();
  bound.x = tex_w, bound.y = tex_h;
  Array<Vector2I> sprite_ranges;
  for (const auto &sprite_range : sprite_ranges_) {
    sprite_ranges.push_back(sprite_range.range.size);
  }
  Rect2DI target_rect = func::algo::RectPacking::GetNextAvailableRect(bound, sprite_ranges, {width, height});
  return AppendSprite(id, data, target_rect);
}

bool Texture2D::AppendSprite(core::StringView name, char *data, UInt32 width, UInt32 height) {
  PROFILE_SCOPE_AUTO;
  return AppendSprite(name.GetHashCode(), data, width, height);
}

bool Texture2D::AppendSprite(core::StringView name, core::StringView path) {
  PROFILE_SCOPE_AUTO;
  return AppendSprite(name.GetHashCode(), path);
}

bool Texture2D::AppendSprite(UInt64 id, StringView path) {
  PROFILE_SCOPE_AUTO;
  if (!File::IsExist(path)) {
    LOGGER.Error("Resource.Texture2D", "AppendSprite: 文件{}不存在", path);
    return false;
  }
  Int32 width = 0, height = 0, channels = 0;
  stbi_uc *pixels = LoadImageStb(path, &width, &height, &channels);
  auto num_channel = GetNumChannels();
  if (channels != num_channel) {
    if (num_channel == 4 && channels == 3) {
      auto new_pixels = ConvertChannels(pixels, width, height, channels, num_channel);
      stbi_image_free(pixels);
      bool success = AppendSprite(id, reinterpret_cast<char *>(new_pixels), width, height);
      Free(new_pixels);
      return success;
    }
    LOGGER.Error("Resource.Texture2D", "AppendSprite: 文件{}的通道数不匹配, [S={}, T={}]", path, channels,
                 GetNumChannels());
    return false;
  }
  bool success = AppendSprite(id, reinterpret_cast<char *>(pixels), width, height);
  stbi_image_free(pixels);
  return success;
}

void Texture2D::Download() const {
  PROFILE_SCOPE_AUTO;
  if (GetAssetPath().IsEmpty()) {
    LOGGER.Error("Resource.Texture2D", "下载纹理失败: 资源路径为空");
    return;
  }
  // 将图像数据拉取下来保存到新的图片中
  auto w = GetWidth();
  auto h = GetHeight();
  auto num_channels = GetNumChannels();
  auto cpu_buffer = native_image_->CreateCPUVisibleBuffer();
  void *data = cpu_buffer->BeginRead();
  stbi_write_png(*GetAssetPath(), w, h, num_channels, data, w * num_channels);
  cpu_buffer->EndRead(data);
}

UInt8 *Texture2D::ConvertChannels(UInt8 *data, UInt32 width, UInt32 height, UInt32 src_channels, UInt32 dst_channels) {
  if (src_channels == dst_channels)
    return data;
  const UInt64 required_size = width * height * dst_channels;
  const auto result = static_cast<UInt8 *>(Malloc(required_size));
  if (src_channels == 3 && dst_channels == 4) {
    for (UInt64 i = 0; i < width * height; i++) {
      result[i * 4 + 0] = data[i * 3 + 0];
      result[i * 4 + 1] = data[i * 3 + 1];
      result[i * 4 + 2] = data[i * 3 + 2];
      result[i * 4 + 3] = 255;
    }
  }
  return result;
}

String Texture2D::GetSpriteRangeString() const {
  String sprite_range_str;
  for (auto sprite_range : sprite_ranges_) {
    sprite_range_str +=
        String::Format("{}:{}-{}-{}-{}\n", sprite_range.id, sprite_range.range.position.x,
                       sprite_range.range.position.y, sprite_range.range.size.x, sprite_range.range.size.y);
  }
  return sprite_range_str;
}

void Texture2D::SetSpriteRangeString(core::StringView str) {
  auto sprite_range_lines = str.Split();
  for (auto sprite_range_line : sprite_range_lines) {
    auto id_range = sprite_range_line.Split(":");
    if (id_range.size() != 2) {
      LOGGER.Warn("Resource.Texture2D", "SetSpriteRangeString: sprite range line {}解析失败!", sprite_range_line);
      continue;
    }
    SpriteRange range;
    UInt64 id;
    auto result = std::from_chars(id_range[0].Data(), id_range[0].Data() + id_range[0].Length(), id);
    if (result.ec != std::errc()) {
      LOGGER.Warn("Resource.Texture2D", "SetSpriteRangeString: sprite range line {} id解析失败!", sprite_range_line);
      continue;
    }
    range.id = id;
    auto range_str = id_range[1].Split("-");
    if (range_str.size() != 4) {
      LOGGER.Warn("Resource.Texture2D", "SetSpriteRangeString: sprite range line {} range解析失败!", sprite_range_line);
      continue;
    }
    for (int i = 0; i < 4; i++) {
      Int32 element = 0;
      auto range_result = std::from_chars(range_str[i].Data(), range_str[i].Data() + range_str[i].Length(), element);
      if (range_result.ec != std::errc()) {
        LOGGER.Warn("Resource.Texture2D", "SetSpriteRangeString: sprite range line {} range {}解析失败!",
                    sprite_range_line, i);
        continue;
      }
      if (i == 0)
        range.range.position.x = element;
      if (i == 1)
        range.range.position.y = element;
      if (i == 2)
        range.range.size.x = element;
      if (i == 3)
        range.range.size.y = element;
    }
    sprite_ranges_.push_back(range);
  }
}
