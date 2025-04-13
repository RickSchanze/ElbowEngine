//
// Created by Echo on 2025/3/25.
//

#include "Texture2DMeta.hpp"

IMPL_REFLECTED(Texture2DMeta) {
    return Type::Create<Texture2DMeta>("Texture2DMeta") | refl_helper::Attribute(Type::ValueAttribute::SQLTable, "Texture2D") |
           refl_helper::AddField("Id", &Texture2DMeta::Id).Attribute(Field::ValueAttribute::SQLAttr, "(PrimaryKey, AutoIncrement)") |
           refl_helper::AddField("ObjectHandle", &Texture2DMeta::ObjectHandle) | refl_helper::AddField("Path", &Texture2DMeta::Path) |
           refl_helper::AddField("Format", &Texture2DMeta::Format) | refl_helper::AddField("SpritesString", &Texture2DMeta::SpritesString) |
           refl_helper::AddField("IsCubeMap", &Texture2DMeta::IsCubeMap) | refl_helper::AddField("MipmapLevel", &Texture2DMeta::MipmapLevel);
}
