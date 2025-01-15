//
// Created by Echo on 25-1-14.
//

#pragma once
#include "Core/Base/CoreTypeDef.h"
#include "Platform/RHI/Image.h"

namespace platform::rhi
{
class Image;
}
namespace func
{

/**
 * 获取深度图形的默认描述
 * width或height为0时从主窗口取
 * @param width
 * @param height
 * @return
 */
platform::rhi::ImageDesc GetDepthImageDesc(UInt32 width = 0, UInt32 height = 0);

class RenderTexture
{
public:
    RenderTexture() = default;

    explicit RenderTexture(const platform::rhi::ImageDesc& desc);

    ~RenderTexture();

    /**
     * 判断是否有效
     * @return
     */
    [[nodiscard]] bool IsValid() const { return native_handle_ != nullptr; }

    core::SharedPtr<platform::rhi::Image> GetNativeHandle() const { return native_handle_; }

    [[nodiscard]] UInt32 GetWidth() const { return desc_.width; }
    [[nodiscard]] UInt32 GetHeight() const { return desc_.height; }
    [[nodiscard]] UInt32 GetDepth() const { return desc_.depth_or_layers; }
    [[nodiscard]] UInt32 GetMipLevels() const { return desc_.mip_levels; }

    [[nodiscard]] platform::rhi::Format GetFormat() const { return desc_.format; }

    [[nodiscard]] platform::rhi::ImageView* GetImageView() const { return image_view_.get(); }

    // 下面三个用于设置RenderTexture的宽高, 注意是高耗时操作
    // 因为是销毁后重建, 所以尽量不要频繁调用
    void SetWidth(UInt32 width);
    void SetHeight(UInt32 height);
    void Resize(UInt32 width, UInt32 height);

private:
    core::SharedPtr<platform::rhi::Image>     native_handle_ = nullptr;
    core::SharedPtr<platform::rhi::ImageView> image_view_    = nullptr;

    platform::rhi::ImageDesc desc_ = platform::rhi::ImageDesc::Default();
};

}   // namespace func
