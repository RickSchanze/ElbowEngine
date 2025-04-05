//
// Created by Echo on 2025/3/23.
//
#pragma once
#include "Core/Core.hpp"
#include "Core/Math/Types.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "Core/Profile.hpp"
#include "Platform/RHI/Enums.hpp"
#include "Resource/Assets/Asset.hpp"
#include "Texture2DMeta.hpp"


namespace rhi {
    struct ImageViewDesc;
}
namespace rhi {
    class Image;
    class ImageView;
} // namespace rhi
struct Texture2DMeta;
struct SpriteRange {
    // 表示位置和尺寸, 左上角是(0,0)
    Rect2Di range{};
    // 表示ID, 是使用字符串的hash值
    UInt64 id{};

    bool IsValid() const { return range.size.x != 0 && range.size.y != 0; }
};

class REFLECTED() Texture2D : public Asset {
    REFLECTED_CLASS(Texture2D)
public:
    [[nodiscard]] AssetType GetAssetType() const override { return AssetType::Texture2D; }

    void PerformLoad() override;

    void PerformUnload() override {
        native_image_view_ = nullptr;
        native_image_ = nullptr;
    }

    // TODO: 返回bool 错误处理
    void Load(const Texture2DMeta &meta);
    [[nodiscard]] bool IsLoaded() const override { return static_cast<bool>(native_image_); }

    [[nodiscard]] UInt32 GetWidth() const;

    [[nodiscard]] UInt32 GetHeight() const;

    [[nodiscard]] UInt32 GetNumChannels() const;

    [[nodiscard]] rhi::Format GetFormat() const;

    static Texture2D *GetDefault();

    [[nodiscard]] rhi::ImageView *GetNativeImageView() const { return native_image_view_.get(); }
    [[nodiscard]] rhi::Image *GetNativeImage() const { return native_image_.get(); }

    [[nodiscard]] Rect2Df GetUVRect(const SpriteRange &sprite_range) const;


    [[nodiscard]] SpriteRange GetSpriteRange(const UInt64 id) const;

    [[nodiscard]] SpriteRange GetSpriteRange(const StringView name) const { return GetSpriteRange(name.GetHashCode()); }

    [[nodiscard]] StringView GetAssetPath() const { return meta_.path; }
    TextureUsage GetTextureUsage() const { return meta_.usage; }

#if WITH_EDITOR
    void SetTextureUsage(TextureUsage usage);

    /**
     * 这个函数设置asset_path, 你仅应在下面的场景调用此函数:
     * 1. 需要引擎内生成一个Texture2D作为磁盘上的资产时
     *    例如TextureAtlas是完全动态生成的
     * 2. 待补充
     * 当asset_path被设置时, 调用此函数会触发Assert
     */
    void SetAssetPath(const StringView new_path) {
        if (meta_.dynamic) {
            VLOG_ERROR("只要dynamic的可以修改path, 且修改完后变成非dynamic");
            return;
        }
        if (new_path == GetAssetPath())
            return;
        Assert(GetAssetPath().IsEmpty(), "SetAssetPath: 此纹理已有路径: {}", *GetAssetPath());
        meta_.path = new_path;
        meta_.dynamic = false;
        SetNeedSave();
    }

    /**
     * 增加一个sprite 在指定的target_rect处
     * @param id
     * @param data
     * @param target_rect
     * @return
     */
    bool AppendSprite(UInt64 id, const char *data, Rect2Di target_rect);

    /**
     * 增加一个Sprite 自动计算应该在哪
     * @param id
     * @param data
     * @param width
     * @param height
     * @return
     */
    bool AppendSprite(const UInt64 id, const char *data, const UInt32 width, const UInt32 height);

    bool AppendSprite(const StringView name, const char *data, const UInt32 width, const UInt32 height) {
        ProfileScope _(__func__);
        return AppendSprite(name.GetHashCode(), data, width, height);
    }

    /**
     * 将位于path的图片添加到Texture2D 以name为命名
     * path可以是绝对路径
     * @param name
     * @param path
     * @return
     */
    bool AppendSprite(const StringView name, const StringView path) {
        ProfileScope _(__func__);
        return AppendSprite(name.GetHashCode(), path);
    }

    bool AppendSprite(const UInt64 id, StringView path);

    /**
     * 拉取GPU图像保存到当前的GetAssetPath()中
     */
    void Download() const;

    /**
     * 将data数据通道转换
     * @param data 传入, 如果src_channels == dst_channels 直接返回data, 否则Malloc返回新内存
     * @param width
     * @param height
     * @param src_channels
     * @param dst_channels
     * @note 传入data不会被释放, 返回的新的也需要手动释放
     * @return
     */
    static UInt8 *ConvertChannels(UInt8 *data, const UInt32 width, const UInt32 height, const UInt32 src_channels, const UInt32 dst_channels);

    [[nodiscard]] String GetSpriteRangeString() const;

    void SetSpriteRangeString(const StringView str);

    void Save() override;
#endif

private:
    rhi::Format SelectFormat() const;

    SharedPtr<rhi::Image> native_image_ = nullptr;
    SharedPtr<rhi::ImageView> native_image_view_ = nullptr;

    // 这个Texture2D包含的sprites
    Array<SpriteRange> sprite_ranges_;

    Texture2DMeta meta_;
};
