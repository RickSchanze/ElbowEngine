//
// Created by Echo on 2025/3/25.
//

#include "Texture2D.hpp"
#ifdef ELBOW_PLATFORM_WINDOWS
#include "Windows.h"
#endif
#define STB_IMAGE_IMPLEMENTATION
#include <charconv>
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
#define STB_IMAGE_WRITE_IMPLEMENTATION // 必须定义此宏以启用函数实现
#include "stb_image_write.h"

using namespace rhi;

IMPL_REFLECTED_INPLACE(Texture2D) { return Type::Create<Texture2D>("Texture2D") | refl_helper::AddParents<Asset>(); }

static stbi_uc *LoadImageStb(StringView path, int *width, int *height, int *channels) {
#ifdef ELBOW_PLATFORM_WINDOWS
    const auto str = path.ToWideString();
    FILE *f;
    _wfopen_s(&f, str.c_str(), L"rb");
    if (!f)
        return nullptr;
    stbi_uc *pixels = stbi_load_from_file(f, width, height, channels, 4);
    fclose(f);
    return pixels;
#else
    return nullptr;
#endif
}

void Texture2D::PerformLoad() {
    ProfileScope _(__func__);
    auto meta_op = AssetDataBase::QueryMeta<Texture2DMeta>(GetHandle());
    if (!meta_op) {
        Log(Error) << String("加载失败: handle为{}的Texture2D不在资产数据库", GetHandle());
        return;
    }
    Load(*meta_op);
}

void Texture2D::Load(const Texture2DMeta &meta) {
    ProfileScope _(__func__);
    if (!meta.dynamic) {
        StringView path = meta.path;
        if (!path.EndsWith(".png")) {
            Log(Error) << String::Format("加载失败: Texture2D必须以.png结尾: {}", *path);
            return;
        }
        if (!Path::IsExist(path)) {
            Log(Error) << String::Format("加载失败: 路径为{}的Texture2D文件不存在", *path);
            return;
        }
        Int32 width = 0, height = 0, channels = 0;
        stbi_uc *pixels = stbi_load(*path, &width, &height, &channels, STBI_rgb_alpha);
        if (!pixels) {
            Log(Error) << String::Format("加载失败: 路径为{}的Texture2D文件无法加载", *path);
            stbi_image_free(pixels);
            return;
        }
        const Format format = meta.format;
        const ImageDesc desc{static_cast<size_t>(width), static_cast<size_t>(height), IUB_TransferDst | IUB_ShaderRead, format, ImageDimension::D2};
        String debug_name = String::Format("Texture2D_{}", *path);
        native_image_ = GetGfxContextRef().CreateImage(desc, *debug_name);
        const ImageViewDesc view_desc{native_image_.get()};
        debug_name = String::Format("Texture2DView_{}", *path);
        native_image_view_ = GetGfxContextRef().CreateImageView(view_desc, *debug_name);
        GfxCommandHelper::CopyDataToImage2D(pixels, native_image_.get(), width * height * channels);
        stbi_image_free(pixels);
        asset_path_ = meta.path;
        SetSpriteRangeString(meta.sprites_string);
    } else {
        if (meta.width == 0 || meta.height == 0) {
            Log(Error) << "加载失败: Texture2D的宽度和高度必须大于0";
            return;
        }
        const Format format = meta.format;
        // 这里设为TransferSrc是因为很可能是要作为之后保存的图像创建的
        const ImageDesc desc{meta.width, meta.height, IUB_TransferDst | IUB_ShaderRead | IUB_TransferSrc, format, ImageDimension::D2};
        String debug_name = String::Format("Texture2D_{}", *name_);
        native_image_ = GetGfxContextRef().CreateImage(desc, *debug_name);
        const ImageViewDesc view_desc{native_image_.get()};
        debug_name = String::Format("Texture2DView_{}", *name_);
        native_image_view_ = GetGfxContextRef().CreateImageView(view_desc, *debug_name);
        asset_path_ = meta.path;
        SetSpriteRangeString(meta.sprites_string);
    }
}

UInt32 Texture2D::GetWidth() const {
    if (!native_image_)
        return 0;
    return native_image_->GetWidth();
}

UInt32 Texture2D::GetHeight() const {
    if (!native_image_)
        return 0;
    return native_image_->GetHeight();
}

UInt32 Texture2D::GetNumChannels() const { return native_image_->GetNumChannels(); }

rhi::Format Texture2D::GetFormat() const {
    if (!native_image_)
        return rhi::Format::Count;
    return native_image_->GetFormat();
}

Texture2D *Texture2D::GetDefault() {
    ProfileScope _(__func__);
    const auto rtn = AssetDataBase::Load<Texture2D>("Assets/Texture/Default.png");
    Assert(rtn != nullptr, "Default.png不存在");
    return rtn;
}

Rect2Df Texture2D::GetUVRect(const SpriteRange &sprite_range) const {
    ProfileScope _(__func__);
    Rect2Df uv_rect;
    uv_rect.pos.x = static_cast<Float>(sprite_range.range.pos.x) / static_cast<Float>(GetWidth());
    uv_rect.pos.y = static_cast<Float>(sprite_range.range.pos.y) / static_cast<Float>(GetHeight());
    uv_rect.size.x = static_cast<Float>(sprite_range.range.size.x) / static_cast<Float>(GetWidth());
    uv_rect.size.y = static_cast<Float>(sprite_range.range.size.y) / static_cast<Float>(GetHeight());
    return uv_rect;
}

SpriteRange Texture2D::GetSpriteRange(const UInt64 id) const {
    ProfileScope _(__func__);
    for (const auto &sprite_range: sprite_ranges_) {
        if (sprite_range.id == id) {
            return sprite_range;
        }
    }
    return {};
}

bool Texture2D::AppendSprite(UInt64 id, const char *data, Rect2Di target_rect) {
    ProfileScope _(__func__);
    if (target_rect.pos.x + target_rect.size.x > GetWidth() || target_rect.pos.y + target_rect.size.y > GetHeight()) {
        Log(Error) << "target_rect超出了Texture2D的尺寸";
        return false;
    }
    if (range::AnyOf(sprite_ranges_, [id](const SpriteRange &sprite_range) { return sprite_range.id == id; })) {
        Log(Error) << String::Format("id为{}的SpriteRange已经存在", id);
        return false;
    }
    if (!data) {
        return false;
    }
    // 拷贝数据到图像
    const auto num_channels = GetNumChannels();
    GfxCommandHelper::CopyDataToImage2D(data, native_image_.get(), target_rect.Area() * num_channels, {target_rect.pos.x, target_rect.pos.y, 0},
                                        {target_rect.size.x, target_rect.size.y, 1});
    SpriteRange new_sprite_range{};
    new_sprite_range.id = id;
    new_sprite_range.range = target_rect;
    sprite_ranges_.Add(new_sprite_range);
    return true;
}

bool Texture2D::AppendSprite(const UInt64 id, const char *data, const UInt32 width, const UInt32 height) {
    ProfileScope _(__func__);
    Vector2i bound;
    const UInt32 tex_w = GetWidth();
    const UInt32 tex_h = GetHeight();
    bound.x = tex_w, bound.y = tex_h;
    Array<Vector2i> sprite_ranges;
    for (const auto &sprite_range: sprite_ranges_) {
        sprite_ranges.Add(sprite_range.range.size);
    }
    const Rect2Di target_rect =
            algo::RectPacking::GetNextAvailableRect(bound, sprite_ranges, {static_cast<Int32>(width), static_cast<Int32>(height)});
    return AppendSprite(id, data, target_rect);
}

bool Texture2D::AppendSprite(const UInt64 id, StringView path) {
    ProfileScope _(__func__);
    if (!File::IsExist(path)) {
        Log(Error) << String::Format("AppendSprite: 文件{}不存在", *path);
        return false;
    }
    Int32 width = 0, height = 0, channels = 0;
    stbi_uc *pixels = LoadImageStb(path, &width, &height, &channels);
    auto num_channel = GetNumChannels();
    if (channels != num_channel) {
        if (num_channel == 4 && channels == 3) {
            const auto new_pixels = ConvertChannels(pixels, width, height, channels, num_channel);
            stbi_image_free(pixels);
            const bool success = AppendSprite(id, reinterpret_cast<char *>(new_pixels), width, height);
            Free(new_pixels);
            return success;
        }
        Log(Error) << String::Format("AppendSprite: 文件{}的通道数不匹配, [S={}, T={}]", *path, channels, num_channel);
        return false;
    }
    const bool success = AppendSprite(id, reinterpret_cast<char *>(pixels), width, height);
    stbi_image_free(pixels);
    return success;
}

void Texture2D::Download() const {
    ProfileScope _(__func__);
    if (GetAssetPath().IsEmpty()) {
        Log(Error) << "下载纹理失败: 资源路径为空";
        return;
    }
    // 将图像数据拉取下来保存到新的图片中
    const auto w = GetWidth();
    const auto h = GetHeight();
    const auto num_channels = GetNumChannels();
    const auto cpu_buffer = native_image_->CreateCPUVisibleBuffer();
    const void *data = cpu_buffer->BeginRead();
    stbi_write_png(*GetAssetPath(), w, h, num_channels, data, w * num_channels);
    cpu_buffer->EndRead();
}

UInt8 *Texture2D::ConvertChannels(UInt8 *data, const UInt32 width, const UInt32 height, const UInt32 src_channels, const UInt32 dst_channels) {
    if (src_channels == dst_channels)
        return data;
    const UInt64 required_size = width * height * dst_channels;
    const auto result = static_cast<UInt8 *>(Malloc(required_size));
    if (src_channels == 3 && dst_channels == 4) {
        for (UInt64 i = 0; i < width * height; i++) {
            result[i * 4 + 0] = data[i * 3 + 0];
            result[i * 4 + 1] = data[i * 3 + 1];
            result[i * 4 + 2] = data[i * 3 + 2];
            result[i * 4 + 3] = 255;
        }
    }
    return result;
}

String Texture2D::GetSpriteRangeString() const {
    String sprite_range_str;
    for (auto sprite_range: sprite_ranges_) {
        sprite_range_str += String::Format("{}:{}-{}-{}-{}\n", sprite_range.id, sprite_range.range.pos.x, sprite_range.range.pos.y,
                                           sprite_range.range.size.x, sprite_range.range.size.y);
    }
    return sprite_range_str;
}

void Texture2D::SetSpriteRangeString(const StringView str) {
    auto sprite_range_lines = str.Split();
    for (auto sprite_range_line: sprite_range_lines) {
        auto id_range = sprite_range_line.Split(":");
        if (id_range.Count() != 2) {
            Log(Error) << String::Format("SetSpriteRangeString: sprite range line {}格式不正确!", *sprite_range_line);
            continue;
        }
        SpriteRange range;
        UInt64 id;
        const auto result = std::from_chars(id_range[0].Data(), id_range[0].Data() + id_range[0].ByteCount(), id);
        if (result.ec != std::errc()) {
            Log(Error) << String::Format("SetSpriteRangeString: sprite range line {} id解析失败!", *sprite_range_line);
            continue;
        }
        range.id = id;
        auto range_str = id_range[1].Split("-");
        if (range_str.Count() != 4) {
            Log(Error) << String::Format("SetSpriteRangeString: sprite range line {} range格式不正确!", *sprite_range_line);
            continue;
        }
        for (int i = 0; i < 4; i++) {
            Int32 element = 0;
            if (const auto [ptr, ec] = std::from_chars(range_str[i].Data(), range_str[i].Data() + range_str[i].ByteCount(), element);
                ec != std::errc()) {
                Log(Warn) << String::Format("SetSpriteRangeString: sprite range line {} range {}解析失败!", *sprite_range_line, i);
                continue;
            }
            if (i == 0)
                range.range.pos.x = element;
            if (i == 1)
                range.range.pos.y = element;
            if (i == 2)
                range.range.size.x = element;
            if (i == 3)
                range.range.size.y = element;
        }
        sprite_ranges_.Add(range);
    }
}
