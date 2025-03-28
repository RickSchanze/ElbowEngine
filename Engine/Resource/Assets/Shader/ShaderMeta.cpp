//
// Created by Echo on 2025/3/27.
//
#include "ShaderMeta.hpp"

IMPL_REFLECTED(ShaderMeta) {
    return Type::Create<ShaderMeta>("ShaderMeta") | refl_helper::Attribute(Type::ValueAttribute::SQLTable, "Shader") |
           refl_helper::AddField("id", &ShaderMeta::id).Attribute(Field::ValueAttribute::SQLAttr, "(PrimaryKey, AutoIncrement)") |
           refl_helper::AddField("object_handle", &ShaderMeta::object_handle) | refl_helper::AddField("path", &ShaderMeta::path);
}
