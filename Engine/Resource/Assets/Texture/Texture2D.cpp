//
// Created by Echo on 2025/3/25.
//

#include "Texture2D.hpp"
#ifdef ELBOW_PLATFORM_WINDOWS
#include "Windows.h"
#endif
#define STB_IMAGE_IMPLEMENTATION
#include "Core/FileSystem/File.hpp"
#include "Core/FileSystem/Path.hpp"
#include "Platform/RHI/Buffer.hpp"
#include "Platform/RHI/GfxCommandHelper.hpp"
#include "Platform/RHI/GfxContext.hpp"
#include "Platform/RHI/Image.hpp"
#include "Platform/RHI/ImageView.hpp"
#include "Resource/Algo.hpp"
#include "Resource/AssetDataBase.hpp"
#include "Texture2DMeta.hpp"
#include "stb_image.h"
#include <charconv>
#define STB_IMAGE_WRITE_IMPLEMENTATION // 必须定义此宏以启用函数实现
#include "stb_image_write.h"
#undef min
#undef max
#define TINYEXR_IMPLEMENTATION
#include "Core/Object/ObjectManager.hpp"
#include "tinyexr.h"

using namespace NRHI;

static stbi_uc* LoadImageStb(StringView path, int* width, int* height, int* channels)
{
#ifdef ELBOW_PLATFORM_WINDOWS
    const auto str = path.ToWideString();
    FILE* f;
    _wfopen_s(&f, str.c_str(), L"rb");
    if (!f)
        return nullptr;
    stbi_uc* pixels = stbi_load_from_file(f, width, height, channels, 4);
    fclose(f);
    return pixels;
#else
    return nullptr;
#endif
}

static bool LoadTextureAccordingFormat(const StringView path, Format format, UInt8*& out, Int32& width, Int32& height, Int32& channel,
                                       UInt64& byte_count)
{
    if (path.EndsWith(".exr"))
    {
        if (format == Format::R32G32B32A32_Float)
        {
            const char* err = nullptr;
            Float* out_temp = nullptr;
            Int32 ret = LoadEXR(&out_temp, &width, &height, *path, &err);
            if (ret != TINYEXR_SUCCESS)
            {
                VLOG_ERROR("加载失败: 路径为", *path, "的纹理加载失败, code=", ret);
                FreeEXRErrorMessage(err);
                return false;
            }
            out = reinterpret_cast<UInt8*>(out_temp);
            channel = 4;
            byte_count = width * height * 4 * sizeof(Float);
            return true;
        }
        else
        {
            return false;
        }
    }
    if (path.EndsWith(".hdr"))
    {
        if (format == Format::R32G32B32A32_Float)
        {
            Float* out_temp = stbi_loadf(*path, &width, &height, &channel, 4);
            if (!out_temp)
            {
                VLOG_ERROR("加载失败: 路径为", *path, "的纹理加载失败");
                return false;
            }
            out = reinterpret_cast<UInt8*>(out_temp);
            channel = 4;
            byte_count = width * height * 4 * sizeof(Float);
            return true;
        }
        else
        {
            return false;
        }
    }
    if (path.EndsWith(".png") || path.EndsWith(".jpg"))
    {
        if (format == Format::R8_UNorm)
        {
            stbi_uc* pixels = stbi_load(*path, &width, &height, &channel, STBI_grey);
            if (!pixels)
            {
                Log(Error) << String::Format("加载失败: 路径为{}的Texture2D文件无法加载", *path);
                stbi_image_free(pixels);
                out = nullptr;
                return false;
            }
            byte_count = width * height;
            out = reinterpret_cast<UInt8*>(pixels);
            return true;
        }
        if (format == Format::R8G8B8A8_UNorm || format == Format::R8G8B8A8_SRGB)
        {
            stbi_uc* pixels = stbi_load(*path, &width, &height, &channel, STBI_rgb_alpha);
            if (!pixels)
            {
                Log(Error) << String::Format("加载失败: 路径为{}的Texture2D文件无法加载", *path);
                stbi_image_free(pixels);
                out = nullptr;
                return false;
            }
            byte_count = width * height * 4;
            out = reinterpret_cast<UInt8*>(pixels);
            return true;
        }
        if (format == Format::R8G8B8_UNorm)
        {
            stbi_uc* pixels = stbi_load(*path, &width, &height, &channel, STBI_rgb);
            if (!pixels)
            {
                Log(Error) << String::Format("加载失败: 路径为{}的Texture2D文件无法加载", *path);
                stbi_image_free(pixels);
                out = nullptr;
                return false;
            }
            byte_count = width * height * 3;
            out = reinterpret_cast<UInt8*>(pixels);
            return true;
        }
    }
    VLOG_ERROR("加载失败: 路径为", *path, "的纹理加载失败");
    return false;
}

void Texture2D::PerformLoad()
{
    ProfileScope _(__func__);
    auto meta_op = AssetDataBase::QueryMeta<Texture2DMeta>(GetHandle());
    if (!meta_op)
    {
        Log(Error) << String("加载失败: handle为{}的Texture2D不在资产数据库", GetHandle());
        return;
    }
    Load(*meta_op);
}

void Texture2D::Load(const Texture2DMeta& meta)
{
    ProfileScope _(__func__);
    if (!meta.IsDynamic)
    {
        StringView path = meta.Path;
        if (!path.EndsWith(".png") && !path.EndsWith(".exr") && !path.EndsWith(".hdr") && !path.EndsWith(".jpg"))
        {
            Log(Error) << String::Format("加载失败: Texture2D可用结尾格式: png, jpg, hdr, exr: {}", *path);
            return;
        }
        if (!Path::IsExist(path))
        {
            Log(Error) << String::Format("加载失败: 路径为{}的Texture2D文件不存在", *path);
            return;
        }
        Int32 width = 0, height = 0, channels = 0;
        UInt64 byte_count = 0;
        UInt8* pixels = nullptr;
        if (!LoadTextureAccordingFormat(path, meta.Format, pixels, width, height, channels, byte_count))
        {
            return;
        }
        const ImageDesc desc{static_cast<UInt32>(width),
                             static_cast<UInt32>(height),
                             IUB_TransferDst | IUB_ShaderRead, //
                             meta.Format,
                             ImageDimension::D2, //
                             meta.IsCubeMap ? 6 : 1,
                             static_cast<UInt16>(meta.MipmapLevel)};
        String debug_name = String::Format("Texture2D_{}", *path);
        mNativeImage = GetGfxContextRef().CreateImage(desc, *debug_name);
        const ImageViewDesc view_desc{mNativeImage.get()};
        debug_name = String::Format("Texture2DView_{}", *path);
        native_image_view_ = GetGfxContextRef().CreateImageView(view_desc, *debug_name);
        GfxCommandHelper::CopyDataToImage2D(pixels, mNativeImage.get(), byte_count);
        free(pixels);
        meta_ = meta;
        SetSpriteRangeString(meta.SpritesString);
    }
    else
    {
        if (meta.Width == 0 || meta.Height == 0)
        {
            Log(Error) << "加载失败: Texture2D的宽度和高度必须大于0";
            return;
        }
        const Format format = meta.Format;
        // 这里设为TransferSrc是因为很可能是要作为之后保存的图像创建的
        const ImageDesc desc{meta.Width,
                             meta.Height,
                             IUB_TransferDst | IUB_ShaderRead | IUB_TransferSrc | IUB_RenderTarget | IUB_Storage,
                             format,
                             ImageDimension::D2,
                             meta.IsCubeMap ? 6 : 1,
                             static_cast<UInt16>(meta.MipmapLevel)};
        String debug_name = String::Format("Texture2D_{}", *mName);
        mNativeImage = GetGfxContextRef().CreateImage(desc, *debug_name);
        const ImageViewDesc view_desc{mNativeImage.get()};
        debug_name = String::Format("Texture2DView_{}", *mName);
        native_image_view_ = GetGfxContextRef().CreateImageView(view_desc, *debug_name);
        meta_ = meta;

        SetSpriteRangeString(meta.SpritesString);
    }
}

UInt32 Texture2D::GetWidth() const
{
    if (!mNativeImage)
        return 0;
    return mNativeImage->GetWidth();
}

UInt32 Texture2D::GetHeight() const
{
    if (!mNativeImage)
        return 0;
    return mNativeImage->GetHeight();
}

UInt32 Texture2D::GetNumChannels() const
{
    return mNativeImage->GetNumChannels();
}

UInt32 Texture2D::GetMipLevelCount() const
{
    return mNativeImage->GetMipLevelCount();
}

NRHI::Format Texture2D::GetFormat() const
{
    if (!mNativeImage)
        return NRHI::Format::Count;
    return mNativeImage->GetFormat();
}

Texture2D* Texture2D::GetDefault()
{
    ProfileScope _(__func__);
    const auto rtn = AssetDataBase::LoadFromPath<Texture2D>("Assets/Texture/Default.png");
    Assert(rtn != nullptr, "Default.png不存在");
    return rtn;
}

Rect2Df Texture2D::GetUVRect(const SpriteRange& sprite_range) const
{
    ProfileScope _(__func__);
    Rect2Df uv_rect;
    uv_rect.Pos.X = static_cast<Float>(sprite_range.Range.Pos.X) / static_cast<Float>(GetWidth());
    uv_rect.Pos.Y = static_cast<Float>(sprite_range.Range.Pos.Y) / static_cast<Float>(GetHeight());
    uv_rect.Size.X = static_cast<Float>(sprite_range.Range.Size.X) / static_cast<Float>(GetWidth());
    uv_rect.Size.Y = static_cast<Float>(sprite_range.Range.Size.Y) / static_cast<Float>(GetHeight());
    return uv_rect;
}

SpriteRange Texture2D::GetSpriteRange(const UInt64 id) const
{
    ProfileScope _(__func__);
    for (const auto& sprite_range : sprite_ranges_)
    {
        if (sprite_range.ID == id)
        {
            return sprite_range;
        }
    }
    return {};
}

SharedPtr<NRHI::ImageView> Texture2D::CreateImageView(ImageViewDesc& desc) const
{
    desc.image = mNativeImage.get();
    return GetGfxContext()->CreateImageView(desc);
}

Texture2D* Texture2D::GetDefaultCubeTexture2D()
{
    static Texture2D* DefaultCube = nullptr;
    if (!DefaultCube)
    {
        Texture2DMeta NewMeta;
        NewMeta.IsDynamic = true;
        NewMeta.Width = 1;
        NewMeta.Height = 1;
        NewMeta.Format = Format::R8G8B8A8_UNorm;
        NewMeta.IsCubeMap = true;
        DefaultCube = CreateNewObject<Texture2D>();
        DefaultCube->Load(NewMeta);
    }
    return DefaultCube;
}

void Texture2D::SetTextureFormat(Format format)
{
    if (meta_.Format != format)
    {
        meta_.Format = format;
        SetNeedSave();
        Load(meta_);
    }
}

bool Texture2D::AppendSprite(UInt64 id, const char* data, Rect2Di target_rect)
{
    ProfileScope _(__func__);
    if (target_rect.Pos.X + target_rect.Size.X > GetWidth() || target_rect.Pos.Y + target_rect.Size.Y > GetHeight())
    {
        Log(Error) << "target_rect超出了Texture2D的尺寸";
        return false;
    }
    if (NRange::AnyOf(sprite_ranges_, [id](const SpriteRange& sprite_range) { return sprite_range.ID == id; }))
    {
        Log(Error) << String::Format("id为{}的SpriteRange已经存在", id);
        return false;
    }
    if (!data)
    {
        return false;
    }
    // 拷贝数据到图像
    const auto num_channels = GetNumChannels();
    GfxCommandHelper::CopyDataToImage2D(data, mNativeImage.get(), target_rect.Area() * num_channels, {target_rect.Pos.X, target_rect.Pos.Y, 0},
                                        {target_rect.Size.X, target_rect.Size.Y, 1});
    SpriteRange new_sprite_range{};
    new_sprite_range.ID = id;
    new_sprite_range.Range = target_rect;
    sprite_ranges_.Add(new_sprite_range);
    return true;
}

bool Texture2D::AppendSprite(const UInt64 id, const char* data, const UInt32 width, const UInt32 height)
{
    ProfileScope _(__func__);
    Vector2i bound;
    const UInt32 tex_w = GetWidth();
    const UInt32 tex_h = GetHeight();
    bound.X = tex_w, bound.Y = tex_h;
    Array<Vector2i> sprite_ranges;
    for (const auto& sprite_range : sprite_ranges_)
    {
        sprite_ranges.Add(sprite_range.Range.Size);
    }
    const Rect2Di target_rect =
        NAlgo::RectPacking::GetNextAvailableRect(bound, sprite_ranges, {static_cast<Int32>(width), static_cast<Int32>(height)});
    return AppendSprite(id, data, target_rect);
}

bool Texture2D::AppendSprite(const UInt64 id, StringView path)
{
    ProfileScope _(__func__);
    if (!File::IsExist(path))
    {
        Log(Error) << String::Format("AppendSprite: 文件{}不存在", *path);
        return false;
    }
    Int32 width = 0, height = 0, channels = 0;
    stbi_uc* pixels = LoadImageStb(path, &width, &height, &channels);
    auto num_channel = GetNumChannels();
    if (channels != num_channel)
    {
        if (num_channel == 4 && channels == 3)
        {
            const auto new_pixels = ConvertChannels(pixels, width, height, channels, num_channel);
            stbi_image_free(pixels);
            const bool success = AppendSprite(id, reinterpret_cast<char*>(new_pixels), width, height);
            Free(new_pixels);
            return success;
        }
        Log(Error) << String::Format("AppendSprite: 文件{}的通道数不匹配, [S={}, T={}]", *path, channels, num_channel);
        return false;
    }
    const bool success = AppendSprite(id, reinterpret_cast<char*>(pixels), width, height);
    stbi_image_free(pixels);
    return success;
}

void Texture2D::Download() const
{
    Download(GetAssetPath());
}

void Texture2D::Download(StringView Path) const
{
    ProfileScope _(__func__);
    if (Path.IsEmpty())
    {
        Log(Error) << "下载纹理失败: 资源路径为空";
        return;
    }
    if (Path.EndsWith(".png"))
    {
        // 将图像数据拉取下来保存到新的图片中
        const auto w = GetWidth();
        const auto h = GetHeight();
        const auto num_channels = GetNumChannels();
        const auto cpu_buffer = mNativeImage->CreateCPUVisibleBuffer();
        const void* data = cpu_buffer->BeginRead();
        stbi_write_png(*Path, w, h, num_channels, data, w * num_channels);

        cpu_buffer->EndRead();
    }
    else if (Path.EndsWith(".hdr"))
    {
        // 将图像数据拉取下来保存到新的图片中
        const auto W = GetWidth();
        const auto H = GetHeight();
        constexpr auto NumChannels = 4;
        const auto CpuBuffer = mNativeImage->CreateCPUVisibleBuffer();
        const void* Data = CpuBuffer->BeginRead();
        stbi_write_hdr(*Path, W, H, NumChannels, static_cast<const Float*>(Data));
        CpuBuffer->EndRead();
    }
}

UInt8* Texture2D::ConvertChannels(UInt8* data, const UInt32 width, const UInt32 height, const UInt32 src_channels, const UInt32 dst_channels)
{
    if (src_channels == dst_channels)
        return data;
    const UInt64 required_size = width * height * dst_channels;
    const auto result = static_cast<UInt8*>(Malloc(required_size));
    if (src_channels == 3 && dst_channels == 4)
    {
        for (UInt64 i = 0; i < width * height; i++)
        {
            result[i * 4 + 0] = data[i * 3 + 0];
            result[i * 4 + 1] = data[i * 3 + 1];
            result[i * 4 + 2] = data[i * 3 + 2];
            result[i * 4 + 3] = 255;
        }
    }
    return result;
}

String Texture2D::GetSpriteRangeString() const
{
    String sprite_range_str;
    for (auto sprite_range : sprite_ranges_)
    {
        sprite_range_str += String::Format("{}:{}-{}-{}-{}\n", sprite_range.ID, sprite_range.Range.Pos.X, sprite_range.Range.Pos.Y,
                                           sprite_range.Range.Size.X, sprite_range.Range.Size.Y);
    }
    return sprite_range_str;
}

void Texture2D::SetSpriteRangeString(const StringView str)
{
    auto sprite_range_lines = str.Split();
    for (auto sprite_range_line : sprite_range_lines)
    {
        auto id_range = sprite_range_line.Split(":");
        if (id_range.Count() != 2)
        {
            Log(Error) << String::Format("SetSpriteRangeString: sprite range line {}格式不正确!", *sprite_range_line);
            continue;
        }
        SpriteRange range;
        UInt64 id;
        const auto result = std::from_chars(id_range[0].Data(), id_range[0].Data() + id_range[0].ByteCount(), id);
        if (result.ec != std::errc())
        {
            Log(Error) << String::Format("SetSpriteRangeString: sprite range line {} id解析失败!", *sprite_range_line);
            continue;
        }
        range.ID = id;
        auto range_str = id_range[1].Split("-");
        if (range_str.Count() != 4)
        {
            Log(Error) << String::Format("SetSpriteRangeString: sprite range line {} range格式不正确!", *sprite_range_line);
            continue;
        }
        for (int i = 0; i < 4; i++)
        {
            Int32 element = 0;
            if (const auto [ptr, ec] = std::from_chars(range_str[i].Data(), range_str[i].Data() + range_str[i].ByteCount(), element);
                ec != std::errc())
            {
                Log(Warn) << String::Format("SetSpriteRangeString: sprite range line {} range {}解析失败!", *sprite_range_line, i);
                continue;
            }
            if (i == 0)
                range.Range.Pos.X = element;
            if (i == 1)
                range.Range.Pos.Y = element;
            if (i == 2)
                range.Range.Size.X = element;
            if (i == 3)
                range.Range.Size.Y = element;
        }
        sprite_ranges_.Add(range);
    }
}

void Texture2D::Save()
{
    Super::Save();
    if (auto op_meta = AssetDataBase::QueryMeta<Texture2DMeta>(String::Format("path = '{}'", *GetAssetPath())))
    {
        AssetDataBase::UpdateMeta(meta_);
    }
    else
    {
        VLOG_ERROR("更新前请先导入! handle = ", GetHandle(), " path = ", *GetAssetPath());
    }
}