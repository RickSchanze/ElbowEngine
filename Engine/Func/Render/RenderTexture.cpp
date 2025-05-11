//
// Created by Echo on 2025/3/27.
//

#include "RenderTexture.hpp"

#include "Platform/RHI/GfxCommandHelper.hpp"
#include "Platform/RHI/GfxContext.hpp"
#include "Platform/RHI/ImageView.hpp"
#include "Platform/Window/PlatformWindow.hpp"
#include "Platform/Window/PlatformWindowManager.hpp"
#include "Resource/Assets/Material/Material.hpp"

using namespace NRHI;

ImageDesc GetDepthImageDesc(UInt32 width, UInt32 height)
{
    width = width > 0 ? width : PlatformWindowManager::GetMainWindow()->GetWidth();
    height = height > 0 ? height : PlatformWindowManager::GetMainWindow()->GetHeight();
    return ImageDesc{width, height, IUB_DepthStencil, GetGfxContextRef().GetDefaultDepthStencilFormat()};
}

RenderTexture::RenderTexture(const ImageDesc &desc) : mDesc(desc)
{
    mNativeHandle = GetGfxContextRef().CreateImage(desc);
    Image *image = mNativeHandle.get();
    ImageViewDesc view_desc(image);
    mImageView = GetGfxContextRef().CreateImageView(view_desc);
    if (mDesc.Format == GetGfxContextRef().GetDefaultDepthStencilFormat())
    {
        // 深度图像的Layout一般只变换一次
        ImageSubresourceRange range{};
        range.aspect_mask = IA_Depth;
        range.base_array_layer = 0;
        range.base_mip_level = 0;
        range.layer_count = desc.DepthOrLayers;
        range.level_count = desc.MipLevels;
        GfxCommandHelper::PipelineBarrier(ImageLayout::Undefined, ImageLayout::DepthStencilAttachment, mNativeHandle.get(), range, 0,
                                          AFB_DepthStencilAttachmentWrite, PSFB_TopOfPipe, PSFB_DepthStencilAttachment);
    }
}

RenderTexture::~RenderTexture()
{
    mDesc = ImageDesc::Default();
    mImageView = nullptr;
    mNativeHandle = nullptr;
}

void RenderTexture::SetWidth(UInt32 width)
{
    mNativeHandle = nullptr;
    mDesc.Width = width;
    mNativeHandle = GetGfxContextRef().CreateImage(mDesc);
}

void RenderTexture::SetHeight(UInt32 height)
{
    mNativeHandle = nullptr;
    mDesc.Height = height;
    mNativeHandle = GetGfxContextRef().CreateImage(mDesc);
}

void RenderTexture::Resize(UInt32 width, UInt32 height)
{
    mImageView = nullptr;
    mNativeHandle = nullptr;
    mDesc.Width = width;
    mDesc.Height = height;
    mNativeHandle = GetGfxContextRef().CreateImage(mDesc);
    const ImageViewDesc view_desc(mNativeHandle.get());
    mImageView = GetGfxContextRef().CreateImageView(view_desc);
}

bool RenderTexture::BindToMaterial(const String &name, Material *mat) const
{
    if (mat == nullptr)
        return false;
    return mat->SetParamNativeImageView(name, GetImageView());
}

StaticArray<SharedPtr<ImageView>, 6> RenderTexture::CreateCubeViews() const
{
    return mNativeHandle->CreateCubemapViews();
}