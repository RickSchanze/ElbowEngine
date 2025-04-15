//
// Created by Echo on 2025/3/23.
//
#pragma once


#include "Core/Core.hpp"

REFLECTED(SQLTable = "Mesh")
struct MeshMeta {
    REFLECTED_STRUCT(MeshMeta)

    [[nodiscard]] Int32 GetId() const { return Id; }
    [[nodiscard]] Int32 GetObjectHandle() const { return ObjectHandle; }
    [[nodiscard]] bool GetGenerateNormals() const { return GenerateNormals; }
    [[nodiscard]] bool GetGenerateSmoothNormals() const { return GenerateSmoothNormals; }
    [[nodiscard]] bool GetMergeDuplicateVertices() const { return MergeDuplicateVertices; }
    [[nodiscard]] bool GetRemoveUnusedMaterials() const { return RemoveUnusedMaterials; }
    [[nodiscard]] StringView GetPath() const { return Path; }

    REFLECTED(SQLAttr = "(PrimaryKey, AutoIncrement)")
    Int32 Id = 0;

    REFLECTED()
    Int32 ObjectHandle = 0;

    REFLECTED()
    String Path;

    Float ImportScale = 1.0f;

    REFLECTED(Label = "生成法线", Category = "基础")
    bool GenerateNormals = true;

    REFLECTED(Label = "生产平滑法线", Category = "基础")
    bool GenerateSmoothNormals = false;

    REFLECTED(Label = "合并重复顶点", Category = "基础")
    bool MergeDuplicateVertices = true;

    REFLECTED(Label = "移除多余材质", Category = "基础")
    bool RemoveUnusedMaterials = true;

    REFLECTED(Hidden)
    bool CanBeRemoved = true;

    // TODO: 一些高级选项
};

IMPL_REFLECTED_INPLACE(MeshMeta) {
    return Type::Create<MeshMeta>("MeshMeta") | refl_helper::Attribute(Type::ValueAttribute::SQLTable, "Mesh") |
           refl_helper::AddField("Id", &MeshMeta::Id).Attribute(Field::ValueAttribute::SQLAttr, "(PrimaryKey, AutoIncrement)") | //
           refl_helper::AddField("ObjectHandle", &MeshMeta::ObjectHandle) | //
           refl_helper::AddField("Path", &MeshMeta::Path) | //
           refl_helper::AddField("GenerateNormals", &MeshMeta::GenerateNormals) | //
           refl_helper::AddField("GenerateSmoothNormals", &MeshMeta::GenerateSmoothNormals) | //
           refl_helper::AddField("MergeDuplicateVertices", &MeshMeta::MergeDuplicateVertices) | //
           refl_helper::AddField("RemoveUnusedMaterials", &MeshMeta::RemoveUnusedMaterials) | //
           refl_helper::AddField("CanBeRemoved", &MeshMeta::CanBeRemoved) | //
           refl_helper::AddField("import_scale", &MeshMeta::ImportScale);
}
