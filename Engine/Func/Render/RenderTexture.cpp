//
// Created by Echo on 25-1-14.
//

#include "RenderTexture.h"

#include "Platform/RHI/CommandBuffer.h"
#include "Platform/RHI/GfxCommandHelper.h"
#include "Platform/RHI/GfxContext.h"
#include "Platform/RHI/ImageView.h"
#include "Platform/Window/Window.h"
#include "Platform/Window/WindowManager.h"

using namespace core;
using namespace func;
using namespace platform::rhi;
using namespace platform;

ImageDesc func::GetDepthImageDesc(UInt32 width, UInt32 height)
{
    width  = width > 0 ? width : GetWindowManager().GetMainWindow()->GetWidth();
    height = height > 0 ? height : GetWindowManager().GetMainWindow()->GetHeight();
    return ImageDesc{width, height, IUB_DepthStencil, GetGfxContextRef().GetDefaultDepthStencilFormat()};
}

RenderTexture::RenderTexture(const ImageDesc& desc) : desc_(desc)
{
    native_handle_      = GetGfxContextRef().CreateImage(desc);
    Image*        image = native_handle_.get();
    ImageViewDesc view_desc(image);
    image_view_ = GetGfxContextRef().CreateImageView(view_desc);
    if (desc_.format == GetGfxContextRef().GetDefaultDepthStencilFormat())
    {
        // 深度图像的Layout一般只变换一次
        ImageSubresourceRange range{};
        range.aspect_mask      = IA_Depth;
        range.base_array_layer = 0;
        range.base_mip_level   = 0;
        range.layer_count      = 1;
        range.level_count      = 1;
        GfxCommandHelper::PipelineBarrier(
            ImageLayout::Undefined,
            ImageLayout::DepthStencilAttachment,
            native_handle_.get(),
            range,
            0,
            AFB_DepthStencilAttachmentWrite,
            PSFB_TopOfPipe,
            PSFB_DepthStencilAttachment
        );
    }
}

RenderTexture::~RenderTexture()
{
    desc_          = ImageDesc::Default();
    image_view_    = nullptr;
    native_handle_ = nullptr;
}

void RenderTexture::SetWidth(UInt32 width)
{
    native_handle_ = nullptr;
    desc_.width    = width;
    native_handle_ = GetGfxContextRef().CreateImage(desc_);
}

void RenderTexture::SetHeight(UInt32 height)
{
    native_handle_ = nullptr;
    desc_.height   = height;
    native_handle_ = GetGfxContextRef().CreateImage(desc_);
}

void RenderTexture::Resize(UInt32 width, UInt32 height)
{
    image_view_    = nullptr;
    native_handle_ = nullptr;
    desc_.width    = width;
    desc_.height   = height;
    native_handle_ = GetGfxContextRef().CreateImage(desc_);
    const ImageViewDesc view_desc(native_handle_.get());
    image_view_ = GetGfxContextRef().CreateImageView(view_desc);
}