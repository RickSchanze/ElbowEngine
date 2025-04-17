//
// Created by Echo on 2025/3/25.
//

#include "Font.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "FontMeta.hpp"
#include "ft2build.h"
#include FT_FREETYPE_H
#include "Core/Math/Math.hpp"
#include "utf8.h"

#include "Core/Object/ObjectManager.hpp"
#include "Platform/RHI/GfxCommandHelper.hpp"
#include "Resource/AssetDataBase.hpp"
#include "Resource/Assets/Material/MaterialMeta.hpp"
#include "Resource/Assets/Shader/Shader.hpp"

#include "Resource/Assets/Material/Material.hpp"
#include "Resource/Assets/Texture/Texture2D.hpp"
#include "Resource/Assets/Texture/Texture2DMeta.hpp"

class FontLib : public Singleton<FontLib> {
public:
    FontLib() {
        Int32 errcode = FT_Init_FreeType(&library_);
        Assert(errcode == 0, "初始化FreeType失败, 错误码={}.", errcode);
    }

    static Int32 Load(const StringView path, FT_Face &face) { return FT_New_Face(GetByRef().library_, *path, 0, &face); }

    static void SetFontSize(const FT_Face face, const Int32 size) { FT_Set_Pixel_Sizes(face, size, size); }

    static Int32 LoadChar(const FT_Face face, const UInt32 unicode) {
        const Int32 errcode = FT_Load_Char(face, unicode, FT_LOAD_RENDER);
        return errcode;
    }

    static Int32 SetCharMapUnicode(const FT_Face face) { return FT_Select_Charmap(face, FT_ENCODING_UNICODE); }

private:
    FT_Library library_ = nullptr;
};

struct Font::FontHandle {
    FT_Face face = nullptr;

    [[nodiscard]] bool IsLoaded() const { return face != nullptr; }

    FontHandle(StringView path, const Int32 size) {
        if (Int32 errcode = FontLib::Load(path, face); errcode != 0) {
            Log(Error) << String::Format("加载字体{}失败, 错误码={}", path, errcode);
        } else {
            FontLib::SetFontSize(face, size);
        }
    }

    ~FontHandle() { FT_Done_Face(face); }
};

Font *Font::GetDefaultFont() {
    return AssetDataBase::LoadFromPath<Font>("Assets/Font/MapleMono.ttf");
}

Material *Font::GetDefaultFontMaterial() {
    struct DefaultMatLoader {
        DefaultMatLoader() {
            const Font *f = Font::GetDefaultFont();
            Assert(f, "默认字体无法加载");
            auto mat_path = "Assets/Material/DefaultFont.mat";
            if (auto meta = AssetDataBase::QueryMeta<MaterialMeta>(String::Format("path = '{}'", mat_path))) {
                Material *m = AssetDataBase::LoadFromPath<Material>((*meta).Path);
                m->SetTexture2D("atlas", f->InternalGetFontAtlas());
                font_mat = m;
            } else {
                const auto shader_path = "Assets/Shader/Text.slang";
                const Shader *text_shader = AssetDataBase::LoadFromPath<Shader>(shader_path);
                Material *font_material = ObjectManager::CreateNewObject<Material>();
                font_material->SetShader(text_shader);
                AssetDataBase::CreateAsset(font_material, mat_path);
                font_mat = font_material;
            }
        }

        Material *font_mat;
    };
    static DefaultMatLoader loader;
    return loader.font_mat;
}

Font::~Font() {
    font_atlas_ = nullptr;
    Delete(dynamic_font_handle_);
    dynamic_font_handle_ = nullptr;
}

void Font::PerformLoad() {
    auto op_meta = AssetDataBase::QueryMeta<FontMeta>(GetHandle());
    if (!op_meta) {
        Log(Error) << String::Format("加载失败: handle为{}的Font不在资产数据库", GetHandle());
        return;
    }
    auto &meta = *op_meta;
    Load(meta);
    SetName(meta.Path);
}

void Font::PerformUnload()
{
    Super::PerformUnload();
    font_atlas_ = nullptr;
}

bool Font::Load(const FontMeta &meta) {
    path_ = meta.Path;
    font_atlas_width_ = meta.FontAtlasWidth;
    font_atlas_height_ = meta.FontAtlasHeight;
    font_size_ = meta.FontSize;
    if (Math::WillMultiplyOverflow(font_atlas_width_, font_atlas_height_)) {
        Log(Error) << "字体图集尺寸过大";
        return false;
    }
    Texture2DMeta atlas_meta;
    atlas_meta.Width = font_atlas_width_;
    atlas_meta.Height = font_atlas_height_;
    atlas_meta.IsDynamic = true;
    atlas_meta.Format = RHI::Format::R8_SRGB;
    auto *font_atlas = ObjectManager::CreateNewObject<Texture2D>();
    font_atlas->SetName(String::Format("Font Atlas for {}", *path_));
    font_atlas->Load(atlas_meta);
    font_atlas_ = font_atlas;
    dynamic_font_handle_ = New<FontHandle>(path_, font_size_);
    return IsLoaded();
}

bool Font::IsLoaded() const {
    const Texture2D *atlas = font_atlas_;
    return atlas && atlas->IsLoaded() && dynamic_font_handle_ && dynamic_font_handle_->IsLoaded();
}

void Font::RequestLoadGlyphs(const StringView &str) {
    if (!IsLoaded()) {
        Log(Error) << "Font尚未加载";
        return;
    }
    // 初始化迭代器
    char *char_begin = const_cast<char *>(str.Data());
    char *char_end = char_begin + str.ByteCount();

    // 遍历并输出码点
    while (char_begin < char_end) {
        try {
            const uint32_t codepoint = utf8::next(char_begin, char_end);
            RequestLoadGlyph(codepoint);
        } catch (const utf8::exception &e) {
            // 处理非法编码
            Log(Error) << String::Format("编码错误: {}", e.what());
            break;
        }
    }
}

void Font::RequestLoadGlyph(UInt32 code_point) {
    if (glyphs_.Contains(code_point))
        return;
    if (UInt32 errcode = FontLib::LoadChar(dynamic_font_handle_->face, code_point); errcode != 0) {
        Log(Warn) << String::Format("加载字符{}失败(<-这是Unicode代码), 错误码={}.", code_point, errcode);
        return;
    }
    const FT_GlyphSlot slot = dynamic_font_handle_->face->glyph;
    const FT_Bitmap &bitmap = slot->bitmap;
    if (cursor_.X + bitmap.width >= font_atlas_width_) {
        cursor_.X = 0;
        cursor_.Y += bitmap.rows;
    }
    if (cursor_.Y + bitmap.rows >= font_atlas_height_) {
        Log(Error) << String::Format("字体{}的字体图集不够大.", path_);
        return;
    }
    Array<UInt8> data;
    data.Reserve(bitmap.rows * bitmap.width);
    for (Int32 y = 0; y < bitmap.rows; y++) {
        for (Int32 x = 0; x < bitmap.width; x++) {
            data.Add(bitmap.buffer[x + y * bitmap.pitch]);
        }
    }
    RHI::GfxCommandHelper::CopyDataToImage2D(data.Data(), font_atlas_->GetNativeImage(), data.Count(), {cursor_.X, cursor_.Y, 0},
                                             {static_cast<Int32>(bitmap.width), static_cast<Int32>(bitmap.rows), 1});
    // 保存此字符的信息
    FontCharacterInfo info{};
    // 这里uv原点在左上角 再加上半像素矫正
    info.uv.pos.X = (static_cast<Float>(cursor_.X) + 0.5f) / static_cast<Float>(font_atlas_width_);
    info.uv.pos.Y = (static_cast<Float>(cursor_.Y) + 0.5f) / static_cast<Float>(font_atlas_height_);
    info.uv.size.X = (static_cast<Float>(bitmap.width) - 1.f) / static_cast<Float>(font_atlas_width_);
    info.uv.size.Y = (static_cast<Float>(bitmap.rows) - 1.f) / static_cast<Float>(font_atlas_height_);
    info.bearing.X = slot->bitmap_left;
    info.bearing.Y = slot->bitmap_top;
    info.advance = slot->advance.x;
    info.size.X = bitmap.width;
    info.size.Y = bitmap.rows;
    cursor_.X += bitmap.width;
    glyphs_[code_point] = info;
}
