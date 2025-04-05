//
// Created by Echo on 2025/3/23.
//
#pragma once
#include "Core/Core.hpp"
#include "Platform/RHI/Enums.hpp"

enum class TextureUsage {
    Normal, // 法线贴图
    Roughness, // 粗糙度贴图
    Metallic, // 金属度贴图
    AO, // AO贴图
    Color,
};
DECL_ENUM_REFL(TextureUsage);
REGISTER_ENUM_TYPE(TextureUsage)

struct REFLECTED(SQLTable = "Texture2D") Texture2DMeta {
    REFLECTED_STRUCT(Texture2DMeta)

    REFLECTED(SQLAttr = "(PrimaryKey, AutoIncrement)")
    Int32 id{};

    REFLECTED()
    Int32 object_handle{};

    REFLECTED()
    String path;

    REFLECTED()
    rhi::Format format = rhi::Format::R8G8B8A8_SRGB;

    // 这个Texture2D包含的所有Sprite
    REFLECTED()
    String sprites_string;

    TextureUsage usage = TextureUsage::Color;


    // 动态创建的Texture2D的参数
    bool dynamic = false;
    UInt32 width = 0;
    UInt32 height = 0;
};

REGISTER_TYPE(Texture2DMeta)
