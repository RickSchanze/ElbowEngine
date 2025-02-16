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
#include GEN_HEADER("Resource.Texture2D.generated.h")

GENERATED_SOURCE()

using namespace core;
using namespace resource;
using namespace platform;
using namespace rhi;

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
    stbi_uc *pixels = stbi_load(*path, &width, &height, &channels, channels);
    if (!pixels) {
      LOGGER.Error(logcat::Resource, "加载失败: 路径为{}的Texture2D文件无法加载", path);
      stbi_image_free(pixels);
      return;
    }
    Format format = meta.format;
    ImageDesc desc{static_cast<size_t>(width), static_cast<size_t>(height), IUB_TransferDst | IUB_ShaderRead, format,
                   ImageDimension::D2};
    String debug_name = String::Format("Texture2D_{}", path);
    native_image_ = GetGfxContextRef().CreateImage(desc, debug_name);
    ImageViewDesc view_desc{native_image_.get()};
    debug_name = String::Format("Texture2DView_{}", path);
    native_image_view_ = GetGfxContextRef().CreateImageView(view_desc, debug_name);
    GfxCommandHelper::CopyDataToImage2D(pixels, native_image_.get(), width * height * channels);
    stbi_image_free(pixels);
  } else {
    if (meta.width == 0 || meta.height == 0) {
      LOGGER.Error(logcat::Resource, "加载失败: Texture2D的宽度和高度必须大于0");
      return;
    }
    Format format = meta.format;
    ImageDesc desc{meta.width, meta.height, IUB_TransferDst | IUB_ShaderRead, format, ImageDimension::D2};
    String debug_name = String::Format("Texture2D_{}", name_);
    native_image_ = GetGfxContextRef().CreateImage(desc, debug_name);
    ImageViewDesc view_desc{native_image_.get()};
    debug_name = String::Format("Texture2DView_{}", name_);
    native_image_view_ = GetGfxContextRef().CreateImageView(view_desc, debug_name);
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

UInt32 Texture2D::GetNumChannels() const {
  PROFILE_SCOPE_AUTO;
  Format f = GetFormat();
  if (f == Format::Count)
    return 0;
  switch (f) {
  case Format::D32_Float:
    return 1;
  case Format::R32G32B32_Float:
    return 3;
  case Format::R8G8B8A8_UNorm:
    return 4;
  default:
    return 0;
  }
}

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

bool Texture2D::AppendSprite(UInt64 id, char *data, Rect2DI target_rect) {
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
  GfxCommandHelper::CopyDataToImage2D(data, native_image_.get(), target_rect.Area() * GetNumChannels(),
                                      {target_rect.position.x, target_rect.position.y, 0},
                                      {target_rect.size.x, target_rect.size.y, 1});
  return true;
}

void Texture2D::AppendSprite(char *data, UInt32 width, UInt32 height) {}