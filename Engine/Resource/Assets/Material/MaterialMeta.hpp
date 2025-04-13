//
// Created by Echo on 2025/3/23.
//
#pragma once
#include "Core/Core.hpp"

struct REFLECTED(SQLTable = "Material") MaterialMeta {
    REFLECTED_STRUCT(MaterialMeta)

    REFLECTED(SQLAttr = "(PrimaryKey, AutoIncrement)")
    Int32 Id{};

    REFLECTED()
    Int32 ObjectHandle{};

    REFLECTED()
    String Path;
};


IMPL_REFLECTED_INPLACE(MaterialMeta) {
    return Type::Create<MaterialMeta>("MaterialMeta") | refl_helper::Attribute(Type::ValueAttribute::SQLTable, "Material") | //
           refl_helper::AddField("Id", &MaterialMeta::Id).Attribute(Field::ValueAttribute::SQLAttr, "(PrimaryKey, AutoIncrement)") | //
           refl_helper::AddField("ObjectHandle", &MaterialMeta::ObjectHandle) | //
           refl_helper::AddField("Path", &MaterialMeta::Path);
}
