//
// Created by Echo on 25-1-15.
//

#pragma once
#include "Resource/Assets/Asset.h"

#include "Core/Math/MathTypes.h"
#include "Platform/RHI/Enums.h"
#include GEN_HEADER("Resource.Texture2D.generated.h")

namespace resource {
class Texture2DMeta;
}
namespace platform::rhi {
class ImageView;
}
namespace platform::rhi {
class Image;
}
namespace resource {

// SpriteRange是一个Texture2D Atlas的子区域
struct SpriteRange {
  // 表示位置和尺寸, 左上角是(0,0)
  core::Rect2DI range{};
  // 表示ID, 是使用字符串的hash值
  UInt64 id{};
};

class CLASS() Texture2D : public Asset {
  GENERATED_CLASS(Texture2D)
public:
  [[nodiscard]] AssetType GetAssetType() const override { return AssetType::Texture2D; }

  void PerformLoad() override;
  void PerformUnload() override;

  // TODO: 返回bool 错误处理
  void Load(const Texture2DMeta &meta);

  [[nodiscard]] bool IsLoaded() const override { return native_image_ != nullptr; }

  [[nodiscard]] UInt32 GetWidth() const;
  [[nodiscard]] UInt32 GetHeight() const;
  [[nodiscard]] UInt32 GetNumChannels() const;
  [[nodiscard]] platform::rhi::Format GetFormat() const;

  static Texture2D *GetDefault();

  [[nodiscard]] platform::rhi::ImageView *GetNativeImageView() const { return native_image_view_.get(); }
  [[nodiscard]] platform::rhi::Image *GetNativeImage() const { return native_image_.get(); }

  [[nodiscard]] core::Rect2D GetUVRect(const SpriteRange &sprite_range) const;
  [[nodiscard]] SpriteRange GetSpriteRange(UInt64 id) const;
  [[nodiscard]] SpriteRange GetSpriteRange(core::StringView name) const;

#if WITH_EDITOR
  /**
   * 增加一个sprite 在指定的target_rect处
   * @param data
   * @param target_rect
   * @param channel
   * @return
   */
  bool AppendSprite(UInt64 id,char* data, core::Rect2DI target_rect);

  /**
   * 增加一个Sprite 自动计算应该在哪
   * @param data
   * @param width
   * @param height
   * @param channel
   * @return
   */
  bool AppendSprite(char* data, UInt32 width, UInt32 height);
#endif

private:
  core::SharedPtr<platform::rhi::Image> native_image_ = nullptr;
  core::SharedPtr<platform::rhi::ImageView> native_image_view_ = nullptr;

  // 这个Texture2D包含的sprites
  core::Array<SpriteRange> sprite_ranges_;
};
} // namespace resource
