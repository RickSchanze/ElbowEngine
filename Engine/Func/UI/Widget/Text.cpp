//
// Created by Echo on 2025/3/29.
//

#include "Text.hpp"

#include "Core/Math/Math.hpp"
#include "Func/Render/Helper.hpp"
#include "Func/UI/Style.hpp"
#include "Func/UI/UiManager.hpp"
#include "Platform/RHI/Commands.hpp"
#include "Platform/RHI/VertexLayout.hpp"
#include "Resource/Assets/Font/Font.hpp"
#include "Resource/Assets/Material/Material.hpp"
#include "Resource/Assets/Material/SharedMaterial.hpp"
#include "Resource/Assets/Texture/Texture2D.hpp"
#include "utf8cpp/utf8.h"

IMPL_REFLECTED(Text) {
    return Type::Create<Text>("Text") | refl_helper::AddParents<Widget>() | //
           refl_helper::AddField("font", &ThisClass::font_) | refl_helper::AddField("font_size", &ThisClass::font_size_) |
           refl_helper::AddField("space", &ThisClass::space_) | refl_helper::AddField("text", &ThisClass::text_) |
           refl_helper::AddField("color", &ThisClass::color_);
}

Text::Text() {
    ProfileScope _("Text::Text");
    font_ = Font::GetDefaultFont();
    color_ = UIManager::GetCurrentStyle().text_color;
    material_ = UIManager::GetDefaultUIFontMaterial();
    font_size_ = ApplyGlobalUIScale(DEFAULT_FONT_SIZE);
    space_ = 2;
}

Text::~Text() {
    ProfileScope _(__func__);
    font_ = nullptr;
    material_ = nullptr;
}

Vector2f Text::GetRebuildRequiredSize() const {
    ProfileScope _(__func__);
    if (text_.IsEmpty()) {
        return Vector2f{};
    }
    const auto text_str = const_cast<char *>(*text_);
    // 初始化迭代器
    char *char_begin = text_str;
    char *char_end = text_str + text_.ByteCount();

    Font *f = font_;
    f->RequestLoadGlyphs(text_);
    const Float font_scale = static_cast<Float>(font_size_) / static_cast<Float>(f->GetFontSize());
    Vector2f result = {};
    // 遍历并输出码点
    while (char_begin < char_end) {
        try {
            if (const UInt32 codepoint = utf8::next(char_begin, char_end); f->HasCharacterInfo(codepoint)) {
                auto &info = f->GetCharacterInfo(codepoint);
                result.x += info.size.x * font_scale + space_;
                result.y = std::max(result.y, info.size.y * font_scale);
            }
        } catch (const utf8::exception &e) {
            // 处理非法编码
            Log(Error) << String::Format("编码错误: {}", e.what());
            break;
        }
    }
    return result;
}

void Text::Rebuild() {
    ProfileScope _(__func__);
    Super::Rebuild();
    if (text_.IsEmpty()) {
        UIManager::RequestWriteData(this, 0, 0);
        return;
    }
    auto text_size = text_.Count();
    auto write = UIManager::RequestWriteData(this, text_size * 4, text_size * 6);

    Font *f = font_;
    const Float font_scale = static_cast<Float>(font_size_) / static_cast<Float>(f->GetFontSize());
    f->RequestLoadGlyphs(text_);
    Vector2f cursor = GetAbsoluteLocation();
    // 初始化迭代器
    auto char_begin = text_.begin();
    auto char_end = text_.end();
    while (char_begin != char_end) {
        try {
            if (const UInt32 codepoint = utf8::next(char_begin, char_end); f->HasCharacterInfo(codepoint)) {
                auto &info = f->GetCharacterInfo(codepoint);
                Rect2Df char_quad;
                char_quad.pos = cursor;
                char_quad.size = {info.size.x * font_scale, info.size.y * font_scale};
                write.AddQuad(char_quad, info.uv, color_);
                cursor.x += info.size.x * font_scale + space_;
            } else {
                VLOG_ERROR("渲染字符", codepoint, "失败");
            }
        } catch (const utf8::exception &e) {
            // 处理非法编码
            Log(Error) << String::Format("编码错误: {}", e.what());
        }
    }
}

void Text::Draw(rhi::CommandBuffer &cmd) {
    ProfileScope _(__func__);
    WidgetVertexIndexBufferInfo *info = UIManager::GetWidgetBufferInfo(this);
    if (info == nullptr) {
        VLOG_ERROR("未找到此Widget对应的Buffer, 是不是忘记调用Rebuild了?");
        return;
    }
    if (info->index_count == 0 || info->vertex_count == 0) {
        return;
    }
    helper::BindMaterial(cmd, GetMaterial());
    cmd.Enqueue<rhi::Cmd_DrawIndexed>(info->index_count, 1, info->index_offset);
}

void Text::SetFontSize(Float now) {
    ProfileScope _(__func__);
    if (font_size_ != now) {
        font_size_ = now;
        SetRebuildDirty(true);
    }
}

void Text::SetText(StringView text) {
    ProfileScope _(__func__);
    if ((StringView) text_ != text) {
        text_ = text;
        SetRebuildDirty(true);
    }
}

void Text::SetColor(Color color) {
    ProfileScope _(__func__);
    if (color_ != color) {
        color_ = color;
        if (const auto write = UIManager::RequestWriteData(this); write.index_count == 0 || write.vertex_count == 0) {
            SetRebuildDirty();
        } else {
            rhi::Vertex_UI *vertex = write.vertex_buffer;
            for (UInt64 i = 0; i < write.vertex_count; ++i) {
                vertex[i].color = color;
            }
        }
    }
}
