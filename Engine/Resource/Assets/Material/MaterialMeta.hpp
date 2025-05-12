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
    return Type::Create<MaterialMeta>("MaterialMeta") | NReflHelper::Attribute(Type::ValueAttribute::SQLTable, "Material") | //
           NReflHelper::AddField("Id", &MaterialMeta::Id).Attribute(Field::ValueAttribute::SQLAttr, "(PrimaryKey, AutoIncrement)") | //
           NReflHelper::AddField("ObjectHandle", &MaterialMeta::ObjectHandle) | //
           NReflHelper::AddField("Path", &MaterialMeta::Path);
}
