//
// Created by Echo on 2025/3/25.
//

#include "Texture2DMeta.hpp"

IMPL_REFLECTED(Texture2DMeta) {
    return Type::Create<Texture2DMeta>("Texture2DMeta") | refl_helper::Attribute(Type::ValueAttribute::SQLTable, "Texture2D") |
           refl_helper::AddField("id", &Texture2DMeta::id).Attribute(Field::ValueAttribute::SQLAttr, "(PrimaryKey, AutoIncrement)") |
           refl_helper::AddField("object_handle", &Texture2DMeta::object_handle) | refl_helper::AddField("path", &Texture2DMeta::path) |
           refl_helper::AddField("format", &Texture2DMeta::format) | refl_helper::AddField("sprites_string", &Texture2DMeta::sprites_string) |
           refl_helper::AddField("usage", &Texture2DMeta::usage);
}

IMPL_ENUM_REFL(TextureUsage) {
    return Type::Create<TextureUsage>("TextureUsage") | refl_helper::AddEnumField("Normal", std::to_underlying(TextureUsage::Normal)) |
           refl_helper::AddEnumField("Roughness", std::to_underlying(TextureUsage::Roughness)) |
           refl_helper::AddEnumField("Metallic", std::to_underlying(TextureUsage::Metallic)) |
           refl_helper::AddEnumField("Color", std::to_underlying(TextureUsage::Color)) |
           refl_helper::AddEnumField("AO", std::to_underlying(TextureUsage::AO));
}
