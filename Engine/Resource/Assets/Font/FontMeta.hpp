//
// Created by Echo on 2025/3/23.
//
#pragma once

#include "Core/Core.hpp"

struct REFLECTED(SQLTable = "Font") FontMeta {
    REFLECTED_STRUCT(FontMeta)
public:
    REFLECTED(SQLAttr = "(PrimaryKey, AutoIncrement)")
    Int32 id{};

    REFLECTED()
    Int32 object_handle{};

    REFLECTED()
    String path;

    REFLECTED()
    Int32 font_atlas_width = 2048;

    REFLECTED()
    Int32 font_atlas_height = 2048;

    REFLECTED()
    Int32 font_size = 32;

    REFLECTED()
    String font_charset_file = "Assets/Font/Charset.txt";
};

IMPL_REFLECTED_INPLACE(FontMeta) {
    return Type::Create<FontMeta>("FontMeta") | refl_helper::Attribute(Type::ValueAttribute::SQLTable, "Font") |
           refl_helper::AddField("id", &FontMeta::id).Attribute(Field::ValueAttribute::SQLAttr, "(PrimaryKey, AutoIncrement)") |
           refl_helper::AddField("object_handle", &FontMeta::object_handle) | refl_helper::AddField("path", &FontMeta::path) |
           refl_helper::AddField("font_atlas_width", &FontMeta::font_atlas_width) |
           refl_helper::AddField("font_atlas_height", &FontMeta::font_atlas_height) | refl_helper::AddField("font_size", &FontMeta::font_size) |
           refl_helper::AddField("font_charset_file", &FontMeta::font_charset_file);
}
