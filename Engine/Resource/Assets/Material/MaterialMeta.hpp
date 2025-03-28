//
// Created by Echo on 2025/3/23.
//
#pragma once
#include "Core/Core.hpp"

struct REFLECTED(SQLTable = "Material") MaterialMeta {
    REFLECTED_STRUCT(MaterialMeta)

    REFLECTED(SQLAttr = "(PrimaryKey, AutoIncrement)")
    Int32 id{};

    REFLECTED()
    Int32 object_handle{};

    REFLECTED()
    String path;
};


IMPL_REFLECTED_INPLACE(MaterialMeta) {
    return Type::Create<MaterialMeta>("MaterialMeta") | refl_helper::Attribute(Type::ValueAttribute::SQLTable, "Material") |
           refl_helper::AddField("id", &MaterialMeta::id).Attribute(Field::ValueAttribute::SQLAttr, "(PrimaryKey, AutoIncrement)") |
           refl_helper::AddField("object_handle", &MaterialMeta::object_handle) | refl_helper::AddField("path", &MaterialMeta::path);
}
