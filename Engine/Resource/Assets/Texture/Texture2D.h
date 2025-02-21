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

  bool IsValid() const;
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

  [[nodiscard]] core::String GetAssetPath() const { return asset_path_; }

#if WITH_EDITOR

  /**
   * 这个函数设置asset_path, 你仅应在下面的场景调用此函数:
   * 1. 需要引擎内生成一个Texture2D作为磁盘上的资产时
   *    例如TextureAtlas是完全动态生成的
   * 2. 待补充
   * 当asset_path被设置时, 调用此函数会触发Assert
   */
  void SetAssetPath(core::StringView new_path);

  /**
   * 增加一个sprite 在指定的target_rect处
   * @param data
   * @param target_rect
   * @return
   */
  bool AppendSprite(UInt64 id, const char *data, core::Rect2DI target_rect);

  /**
   * 增加一个Sprite 自动计算应该在哪
   * @param data
   * @param width
   * @param height
   * @return
   */
  bool AppendSprite(UInt64 id, char *data, UInt32 width, UInt32 height);

  bool AppendSprite(core::StringView name, char *data, UInt32 width, UInt32 height);

  /**
   * 将位于path的图片添加到Texture2D 以name为命名
   * path可以是绝对路径
   * @param name
   * @param path
   * @return
   */
  bool AppendSprite(core::StringView name, core::StringView path);

  bool AppendSprite(UInt64 id, core::StringView path);

  /**
   * 拉取GPU图像保存到当前的GetAssetPath()中
   */
  void Download() const;

  /**
   * 将data数据通道转换
   * @param data 传入, 如果src_channels == dst_channels 直接返回data, 否则Malloc返回新内存
   * @param width
   * @param height
   * @param src_channels
   * @param dst_channels
   * @note 传入data不会被释放, 返回的新的也需要手动释放
   * @return
   */
  static UInt8* ConvertChannels(UInt8* data, UInt32 width, UInt32 height, UInt32 src_channels, UInt32 dst_channels);

  [[nodiscard]] core::String GetSpriteRangeString() const;

  void SetSpriteRangeString(core::StringView str);
#endif

private:
  core::SharedPtr<platform::rhi::Image> native_image_ = nullptr;
  core::SharedPtr<platform::rhi::ImageView> native_image_view_ = nullptr;

  // 这个Texture2D包含的sprites
  core::Array<SpriteRange> sprite_ranges_;

  core::String asset_path_;
};
} // namespace resource
