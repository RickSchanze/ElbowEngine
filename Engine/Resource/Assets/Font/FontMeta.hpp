//
// Created by Echo on 2025/3/23.
//
#pragma once

#include "Core/Core.hpp"

#include GEN_HEADER("FontMeta.generated.hpp")

struct ESTRUCT(SQLTable = "Font") FontMeta {
    GENERATED_BODY(FontMeta)
public:
    EFIELD(SQLAttr = "(PrimaryKey|AutoIncrement)")
    Int32 Id{};


    EFIELD()
    Int32 ObjectHandle{};

    EFIELD()
    String Path;

    EFIELD()
    Int32 FontAtlasWidth = 2048;

    EFIELD()
    Int32 FontAtlasHeight = 2048;

    EFIELD()
    Int32 FontSize = 32;
};
