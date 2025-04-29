//
// Created by Echo on 2025/3/23.
//
#pragma once
#include "Core/Core.hpp"
#include "Core/Math/Rect.hpp"
#include "Core/Object/ObjectPtr.hpp"
#include "Resource/Assets/Asset.hpp"

#include GEN_HEADER("Font.generated.hpp")

class Texture2D;
struct FontMeta;
class Material;
enum class FontRenderMethod {
    // TODO: 实现SDF Font
    SDF,
    Bitmap,
};

struct FontCharacterInfo {
    Rect2Df UV{}; // 字形的UV
    Vector2i Size{}; // 字形的大小
    Vector2i Bearing{}; // 从baseline到字形左部/顶部的便宜
    UInt32 Advance{}; // 从远点到下一个字形原点的距离
};

class ECLASS() Font : public Asset {
    GENERATED_BODY(Font)
public:
    static Font *GetDefaultFont();
    static Material* GetDefaultFontMaterial();

    virtual ~Font() override;
    virtual AssetType GetAssetType() const override
    {
        return AssetType::Font;
    }

    virtual void PerformLoad() override;

    virtual void PerformUnload() override;

    bool Load(const FontMeta& meta);

    virtual bool IsLoaded() const override;

    Int16 GetFontSize() const { return font_size_; }
    /**
     * 设置字体大小, 这会生成新的FontAtlas
     */
    void SetFontSize(const Int16 new_size) {
        if (new_size != font_size_) {
            font_size_ = new_size;
            SetDirty(true);
        }
    }

    Texture2D *GetFontAtlas() const { return font_atlas_; }

    FontRenderMethod GetRenderMethod() const { return render_method_; }
    StringView GetAssetPath() const { return path_; }

    const FontCharacterInfo &GetCharacterInfo(const UInt32 unicode) { return glyphs_[unicode]; }
    bool HasCharacterInfo(const UInt32 unicode) const { return glyphs_.Contains(unicode); }

    void RequestLoadGlyphs(const StringView &str);

    Texture2D *InternalGetFontAtlas() const { return font_atlas_; }

private:
    void RequestLoadGlyph(UInt32 code_point);

    Int32 font_size_ = 20; // 静态字体大小
    FontRenderMethod render_method_ = FontRenderMethod::Bitmap; // 默认不使用SDF
    String path_; // 资源路径
    Int32 font_atlas_width_ = 0; // 字体图集的宽度
    Int32 font_atlas_height_ = 0; // 字体图集的高度

    EFIELD()
    ObjectPtr<Texture2D> font_atlas_ = nullptr; // 字体图集
    Map<UInt32, FontCharacterInfo> glyphs_; // 记录每个字符对应font atlas的信息

    struct FontHandle;
    FontHandle *dynamic_font_handle_ = nullptr; // 当字体是dynamic时有用

    Vector2i cursor_ = {0, 0};
};
