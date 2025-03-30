//
// Created by Echo on 2025/3/29.
//

#pragma once
#include "Core/Core.hpp"
#include "Core/Object/ObjectPtr.hpp"
#include "Widget.hpp"


class Font;
class Text : public Widget {
    REFLECTED_CLASS(Text)

    ObjectPtr<Font> font_ = nullptr;
    UInt8 font_size_ = {32};
    UInt8 space_{2};
    Color color_ = Color::White();
    String text_;

public:
    Text();
    ~Text() override;

    Vector2f GetRebuildRequiredSize() override;
    void Rebuild() override;

    void SetSize(UInt8 now);
    UInt8 GetSize() const { return font_size_; }

    void SetText(StringView text);
    StringView GetText() const { return text_; }

    Color GetColor() const { return color_; }
    void SetColor(Color color);
};

REGISTER_TYPE(Text)
