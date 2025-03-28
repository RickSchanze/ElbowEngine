//
// Created by Echo on 2025/3/23.
//
#pragma once
#include "Core/Core.hpp"
#include "Core/Math/Types.hpp"
#include "Core/Object/ObjectPtr.hpp"
#include "Resource/Assets/Asset.hpp"


class Texture2D;
struct FontMeta;
class Material;
enum class FontRenderMethod {
    // TODO: 实现SDF Font
    SDF,
    Bitmap,
};

struct GlyphInfo {
    Rect2Df uv{};
    UInt32 bearing_x{}, bearing_y{}; // 字符的偏移
    UInt32 width{}, height{};
    UInt32 advance_x{};
};

class REFLECTED() Font : public Asset {
    REFLECTED_CLASS(Font)
public:
    static Font *GetDefaultFont();
    static Material *GetDefaultFontMaterial();

    ~Font() override;
    [[nodiscard]] AssetType GetAssetType() const override { return AssetType::Font; }

    void PerformLoad() override;

    void PerformUnload() override { font_atlas_ = nullptr; }

    bool Load(const FontMeta &meta);

    [[nodiscard]] bool IsLoaded() const override;

    [[nodiscard]] Int16 GetFontSize() const { return font_size_; }
    /**
     * 设置字体大小, 这会生成新的FontAtlas
     */
    void SetFontSize(const Int16 new_size) {
        if (new_size != font_size_) {
            font_size_ = new_size;
            SetDirty(true);
        }
    }

    [[nodiscard]] Texture2D *GetFontAtlas() const { return font_atlas_; }

    [[nodiscard]] FontRenderMethod GetRenderMethod() const { return render_method_; }
    [[nodiscard]] StringView GetAssetPath() const { return path_; }

    [[nodiscard]] GlyphInfo &GetGlyphInfo(const UInt32 unicode) { return glyphs_[unicode]; }
    [[nodiscard]] bool HasGlyph(const UInt32 unicode) const { return glyphs_.Contains(unicode); }

    void RequestLoadGlyphs(const StringView &str);

    Texture2D *InternalGetFontAtlas() const { return font_atlas_; }

private:
    void RequestLoadGlyph(UInt32 code_point);

    Int32 font_size_ = 32; // 静态字体大小
    FontRenderMethod render_method_ = FontRenderMethod::Bitmap; // 默认不使用SDF
    String path_; // 资源路径
    Int32 font_atlas_width_ = 0; // 字体图集的宽度
    Int32 font_atlas_height_ = 0; // 字体图集的高度
    ObjectPtr<Texture2D> font_atlas_ = nullptr; // 字体图集
    Map<UInt32, GlyphInfo> glyphs_; // 记录每个字符对应font atlas的信息

    struct FontHandle;
    FontHandle *dynamic_font_handle_ = nullptr; // 当字体是dynamic时有用

    Vector2ui cursor_ = {0, 0};
};

IMPL_REFLECTED_INPLACE(Font) { return Type::Create<Font>("Font") | refl_helper::AddParents<Asset>(); }
