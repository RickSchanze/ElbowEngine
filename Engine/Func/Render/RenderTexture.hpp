//
// Created by Echo on 2025/3/27.
//

#pragma once
#include "Core/Math/Vector.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "Core/TypeAlias.hpp"
#include "Platform/RHI/Enums.hpp"
#include "Platform/RHI/Image.hpp"


class Material;

namespace RHI
{
class ImageView;
}

/**
 * 获取深度图形的默认描述
 * width或height为0时从主窗口取
 * @param width
 * @param height
 * @return
 */
RHI::ImageDesc GetDepthImageDesc(UInt32 width = 0, UInt32 height = 0);

class RenderTexture
{
public:
    RenderTexture() = default;

    explicit RenderTexture(const RHI::ImageDesc &desc);

    ~RenderTexture();

    /**
     * 判断是否有效
     * @return
     */
    bool IsValid() const
    {
        return mNativeHandle != nullptr;
    }

    RHI::Image *GetImage() const
    {
        return mNativeHandle.get();
    }

    UInt32 GetWidth() const
    {
        return mDesc.Width;
    }

    UInt32 GetHeight() const
    {
        return mDesc.Height;
    }

    UInt32 GetDepth() const
    {
        return mDesc.DepthOrLayers;
    }

    UInt32 GetMipLevels() const
    {
        return mDesc.MipLevels;
    }

    Vector2f GetSize() const
    {
        return Vector2f{static_cast<Float>(mDesc.Width), static_cast<Float>(mDesc.Height)};
    }

    RHI::Format GetFormat() const
    {
        return mDesc.Format;
    }

    RHI::ImageView *GetImageView() const
    {
        return mImageView.get();
    }

    // 下面三个用于设置RenderTexture的宽高, 注意是高耗时操作
    // 因为是销毁后重建, 所以尽量不要频繁调用
    void SetWidth(UInt32 width);

    void SetHeight(UInt32 height);

    void Resize(UInt32 width, UInt32 height);

    bool BindToMaterial(const String &name, Material *mat) const;

    StaticArray<SharedPtr<RHI::ImageView>, 6>  CreateCubeViews() const;

private:
    SharedPtr<RHI::Image> mNativeHandle = nullptr;
    SharedPtr<RHI::ImageView> mImageView = nullptr;

    RHI::ImageDesc mDesc = RHI::ImageDesc::Default();
};