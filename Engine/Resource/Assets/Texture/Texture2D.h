//
// Created by Echo on 25-1-15.
//

#pragma once
#include "Resource/Assets/Asset.h"

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
class CLASS() Texture2D : public Asset {
  GENERATED_CLASS(Texture2D)
public:
  [[nodiscard]] AssetType GetAssetType() const override { return AssetType::Texture2D; }

  void PerformLoad() override;
  void PerformUnload() override;

  // TODO: 返回bool 错误处理
  void Load(const Texture2DMeta& meta);

  [[nodiscard]] bool IsLoaded() const override { return native_image_ != nullptr; }

  [[nodiscard]] UInt32 GetWidth() const;
  [[nodiscard]] UInt32 GetHeight() const;

  static Texture2D *GetDefault();

  [[nodiscard]] platform::rhi::ImageView *GetNativeImageView() const { return native_image_view_.get(); }

private:
  core::SharedPtr<platform::rhi::Image> native_image_ = nullptr;
  core::SharedPtr<platform::rhi::ImageView> native_image_view_ = nullptr;
};
} // namespace resource
