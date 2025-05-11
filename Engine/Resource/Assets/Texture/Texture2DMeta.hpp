//
// Created by Echo on 2025/3/23.
//
#pragma once
#include "Core/Core.hpp"
#include "Platform/RHI/Enums.hpp"

DECL_ENUM_REFL(TextureUsage);

struct REFLECTED(SQLTable = "Texture2D") Texture2DMeta {
    REFLECTED_STRUCT(Texture2DMeta)

    REFLECTED(SQLAttr = "(PrimaryKey, AutoIncrement)")
    Int32 Id{};

    REFLECTED()
    Int32 ObjectHandle{};

    Int32 MipmapLevel = 1;

    REFLECTED()
    String Path;

    REFLECTED()
    NRHI::Format Format = NRHI::Format::R8G8B8A8_SRGB;

    // 这个Texture2D包含的所有Sprite
    REFLECTED()
    String SpritesString;

    bool IsCubeMap = false;


    // 动态创建的Texture2D的参数
    bool IsDynamic = false;
    UInt32 Width = 0;
    UInt32 Height = 0;
};

REGISTER_TYPE(Texture2DMeta)
