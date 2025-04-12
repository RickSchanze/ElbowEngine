//
// Created by Echo on 2025/3/25.
//

#include "Texture2DMeta.hpp"

IMPL_REFLECTED(Texture2DMeta) {
    return Type::Create<Texture2DMeta>("Texture2DMeta") | refl_helper::Attribute(Type::ValueAttribute::SQLTable, "Texture2D") |
           refl_helper::AddField("id", &Texture2DMeta::id).Attribute(Field::ValueAttribute::SQLAttr, "(PrimaryKey, AutoIncrement)") |
           refl_helper::AddField("object_handle", &Texture2DMeta::object_handle) | refl_helper::AddField("path", &Texture2DMeta::path) |
           refl_helper::AddField("format", &Texture2DMeta::format) | refl_helper::AddField("sprites_string", &Texture2DMeta::sprites_string) |
           refl_helper::AddField("is_cubemap", &Texture2DMeta::is_cubemap) | refl_helper::AddField("mip_level", &Texture2DMeta::mip_level);
}
