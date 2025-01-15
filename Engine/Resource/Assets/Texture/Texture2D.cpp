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

using namespace core;
using namespace resource;
using namespace platform;
using namespace rhi;

void Texture2D::PerformLoad()
{
    auto meta_op = AssetDataBase::QueryMeta<Texture2DMeta>(GetHandle());
    if (!meta_op)
    {
        LOGGER.Error(logcat::Resource, "加载失败: handle为{}的Texture2D不在资产数据库", GetHandle());
        return;
    }
    auto&      meta = *meta_op;
    StringView path = meta.path;
    if (!path.EndsWith(".png"))
    {
        LOGGER.Error(logcat::Resource, "加载失败: Texture2D必须以.png结尾: {}", path);
        return;
    }
    if (!Path::IsExist(path))
    {
        LOGGER.Error(logcat::Resource, "加载失败: 路径为{}的Texture2D文件不存在", path);
        return;
    }
    Int32    width, height, channels;
    stbi_uc* pixels = stbi_load(*path, &width, &height, &channels, 4);
    if (!pixels)
    {
        LOGGER.Error(logcat::Resource, "加载失败: 路径为{}的Texture2D文件无法加载", path);
        stbi_image_free(pixels);
        return;
    }
    ImageDesc desc{
        static_cast<size_t>(width), static_cast<size_t>(height), IUB_TransferDst | IUB_ShaderRead, Format::R8G8B8A8_UNorm, ImageDimension::D2
    };
    String debug_name = String::Format("Texture2D_{}", path);
    native_image_     = GetGfxContextRef().CreateImage(desc, debug_name);
    ImageViewDesc view_desc{native_image_.get()};
    debug_name         = String::Format("Texture2DView_{}", path);
    native_image_view_ = GetGfxContextRef().CreateImageView(view_desc, debug_name);
    GfxCommandHelper::CopyDataToImage2D(pixels, native_image_.get(), width * height * 4);
    stbi_image_free(pixels);
}

void Texture2D::PerformUnload()
{
    native_image_view_ = nullptr;
    native_image_ = nullptr;
}

UInt32 Texture2D::GetWidth() const
{
    if (!native_image_) return 0;
    return native_image_->GetWidth();
}

UInt32 Texture2D::GetHeight() const
{
    if (!native_image_) return 0;
    return native_image_->GetHeight();
}