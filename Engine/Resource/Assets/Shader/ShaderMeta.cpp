//
// Created by Echo on 2025/3/27.
//
#include "ShaderMeta.hpp"

IMPL_REFLECTED(ShaderMeta) {
    return Type::Create<ShaderMeta>("ShaderMeta") | NReflHelper::Attribute(Type::ValueAttribute::SQLTable, "Shader") |
           NReflHelper::AddField("Id", &ShaderMeta::Id).Attribute(Field::ValueAttribute::SQLAttr, "(PrimaryKey, AutoIncrement)") |
           NReflHelper::AddField("ObjectHandle", &ShaderMeta::ObjectHandle) | NReflHelper::AddField("Path", &ShaderMeta::Path);
}
