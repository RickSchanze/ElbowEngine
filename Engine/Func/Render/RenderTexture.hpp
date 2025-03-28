//
// Created by Echo on 2025/3/27.
//

#pragma once
#include "Core/Misc/SharedPtr.hpp"
#include "Core/TypeAlias.hpp"
#include "Platform/RHI/Enums.hpp"
#include "Platform/RHI/Image.hpp"


namespace rhi {
    class ImageView;
}
/**
 * 获取深度图形的默认描述
 * width或height为0时从主窗口取
 * @param width
 * @param height
 * @return
 */
rhi::ImageDesc GetDepthImageDesc(UInt32 width = 0, UInt32 height = 0);

class RenderTexture
{
public:
    RenderTexture() = default;

    explicit RenderTexture(const rhi::ImageDesc& desc);

    ~RenderTexture();

    /**
     * 判断是否有效
     * @return
     */
    bool IsValid() const { return native_handle_ != nullptr; }

    SharedPtr<rhi::Image> GetNativeHandle() const { return native_handle_; }

    UInt32 GetWidth() const { return desc_.width; }
    UInt32 GetHeight() const { return desc_.height; }
    UInt32 GetDepth() const { return desc_.depth_or_layers; }
    UInt32 GetMipLevels() const { return desc_.mip_levels; }

    rhi::Format GetFormat() const { return desc_.format; }

    rhi::ImageView* GetImageView() const { return image_view_.get(); }

    // 下面三个用于设置RenderTexture的宽高, 注意是高耗时操作
    // 因为是销毁后重建, 所以尽量不要频繁调用
    void SetWidth(UInt32 width);
    void SetHeight(UInt32 height);
    void Resize(UInt32 width, UInt32 height);

private:
    SharedPtr<rhi::Image>     native_handle_ = nullptr;
    SharedPtr<rhi::ImageView> image_view_    = nullptr;

    rhi::ImageDesc desc_ = rhi::ImageDesc::Default();
};