//
// Created by Echo on 2025/3/25.
//

#include "Texture2DMeta.hpp"

IMPL_REFLECTED(Texture2DMeta) {
    return Type::Create<Texture2DMeta>("Texture2DMeta") | NReflHelper::Attribute(Type::ValueAttribute::SQLTable, "Texture2D") |
           NReflHelper::AddField("Id", &Texture2DMeta::Id).Attribute(Field::ValueAttribute::SQLAttr, "(PrimaryKey, AutoIncrement)") |
           NReflHelper::AddField("ObjectHandle", &Texture2DMeta::ObjectHandle) | NReflHelper::AddField("Path", &Texture2DMeta::Path) |
           NReflHelper::AddField("Format", &Texture2DMeta::Format) | NReflHelper::AddField("SpritesString", &Texture2DMeta::SpritesString) |
           NReflHelper::AddField("IsCubeMap", &Texture2DMeta::IsCubeMap) | NReflHelper::AddField("MipmapLevel", &Texture2DMeta::MipmapLevel);
}
