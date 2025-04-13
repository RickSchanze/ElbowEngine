//
// Created by Echo on 2025/3/23.
//
#pragma once
#include "Core/Core.hpp"

REFLECTED()
struct ShaderMeta {
    REFLECTED_STRUCT(ShaderMeta)

    REFLECTED(SQLAttr = "(PrimaryKey, AutoIncrement)")
    Int32 Id{};

    REFLECTED()
    Int32 ObjectHandle{};

    REFLECTED()
    String Path;

    // 下面可以写一些Shader可能需要的属性, 例如是否开启blend等
};

REGISTER_TYPE(ShaderMeta)
