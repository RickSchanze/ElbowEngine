//
// Created by Echo on 2025/3/27.
//
#include "ShaderMeta.hpp"

IMPL_REFLECTED(ShaderMeta) {
    return Type::Create<ShaderMeta>("ShaderMeta") | refl_helper::Attribute(Type::ValueAttribute::SQLTable, "Shader") |
           refl_helper::AddField("Id", &ShaderMeta::Id).Attribute(Field::ValueAttribute::SQLAttr, "(PrimaryKey, AutoIncrement)") |
           refl_helper::AddField("ObjectHandle", &ShaderMeta::ObjectHandle) | refl_helper::AddField("Path", &ShaderMeta::Path);
}
