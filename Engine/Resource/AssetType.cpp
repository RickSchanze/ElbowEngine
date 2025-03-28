//
// Created by Echo on 2025/3/27.
//
#include "AssetType.hpp"

IMPL_ENUM(AssetType) {
    return Type::Create<AssetType>("AssetType") | refl_helper::AddEnumField("Mesh", static_cast<Int32>(AssetType::Mesh)) |
           refl_helper::AddEnumField("Shader", static_cast<Int32>(AssetType::Shader)) |
           refl_helper::AddEnumField("Texture2D", static_cast<Int32>(AssetType::Texture2D)) |
           refl_helper::AddEnumField("Material", static_cast<Int32>(AssetType::Material)) |
           refl_helper::AddEnumField("Animation", static_cast<Int32>(AssetType::Animation)) |
           refl_helper::AddEnumField("Audio", static_cast<Int32>(AssetType::Audio)) |
           refl_helper::AddEnumField("Font", static_cast<Int32>(AssetType::Font)) |
           refl_helper::AddEnumField("Scene", static_cast<Int32>(AssetType::Scene)) |
           refl_helper::AddEnumField("Prefab", static_cast<Int32>(AssetType::Prefab)) |
           refl_helper::AddEnumField("Count", static_cast<Int32>(AssetType::Count));
}

EXEC_BEFORE_MAIN() { ReflManager::GetByRef().Register<AssetType>(&ConstructType_AssetType); }
