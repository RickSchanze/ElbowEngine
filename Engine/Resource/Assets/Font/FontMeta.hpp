//
// Created by Echo on 2025/3/23.
//
#pragma once

#include "Core/Core.hpp"

struct REFLECTED(SQLTable = "Font") FontMeta {
    REFLECTED_STRUCT(FontMeta)
public:
    REFLECTED(SQLAttr = "(PrimaryKey, AutoIncrement)")
    Int32 Id{};

    REFLECTED()
    Int32 ObjectHandle{};

    REFLECTED()
    String Path;

    REFLECTED()
    Int32 FontAtlasWidth = 2048;

    REFLECTED()
    Int32 FontAtlasHeight = 2048;

    REFLECTED()
    Int32 FontSize = 32;
};

IMPL_REFLECTED_INPLACE(FontMeta) {
    return Type::Create<FontMeta>("FontMeta") | refl_helper::Attribute(Type::ValueAttribute::SQLTable, "Font") |
           refl_helper::AddField("Id", &FontMeta::Id).Attribute(Field::ValueAttribute::SQLAttr, "(PrimaryKey, AutoIncrement)") |
           refl_helper::AddField("ObjectHandle", &FontMeta::ObjectHandle) | //
           refl_helper::AddField("Path", &FontMeta::Path) | //
           refl_helper::AddField("FontAtlasWidth", &FontMeta::FontAtlasWidth) | //
           refl_helper::AddField("FontAtlasHeight", &FontMeta::FontAtlasHeight) | //
           refl_helper::AddField("FontSize", &FontMeta::FontSize);
}
