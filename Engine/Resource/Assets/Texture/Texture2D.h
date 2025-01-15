//
// Created by Echo on 25-1-15.
//

#pragma once
#include "Resource/Assets/Asset.h"

namespace platform::rhi
{
class ImageView;
}
namespace platform::rhi
{
class Image;
}
namespace resource
{
class Texture2D : public Asset
{
public:
    AssetType GetAssetType() const override { return AssetType::Texture2D; }

    void PerformLoad() override;
    void PerformUnload() override;

    bool IsLoaded() const override { return native_image_ != nullptr; }

    UInt32 GetWidth() const;
    UInt32 GetHeight() const;

private:
    core::SharedPtr<platform::rhi::Image>     native_image_      = nullptr;
    core::SharedPtr<platform::rhi::ImageView> native_image_view_ = nullptr;
};
}   // namespace resource
