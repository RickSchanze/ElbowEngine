//
// Created by Echo on 2025/3/23.
//
#pragma once


#include "Core/Core.hpp"

REFLECTED(SQLTable = "Mesh")
struct MeshMeta {
    REFLECTED_STRUCT(MeshMeta)

    Int32 GetId() const { return Id; }
    Int32 GetObjectHandle() const { return ObjectHandle; }
    bool GetGenerateNormals() const { return GenerateNormals; }
    bool GetGenerateSmoothNormals() const { return GenerateSmoothNormals; }
    bool GetMergeDuplicateVertices() const { return MergeDuplicateVertices; }
    bool GetRemoveUnusedMaterials() const { return RemoveUnusedMaterials; }
    StringView GetPath() const { return Path; }

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
    return Type::Create<MeshMeta>("MeshMeta") | NReflHelper::Attribute(Type::ValueAttribute::SQLTable, "Mesh") |
           NReflHelper::AddField("Id", &MeshMeta::Id).Attribute(Field::ValueAttribute::SQLAttr, "(PrimaryKey, AutoIncrement)") | //
           NReflHelper::AddField("ObjectHandle", &MeshMeta::ObjectHandle) | //
           NReflHelper::AddField("Path", &MeshMeta::Path) | //
           NReflHelper::AddField("GenerateNormals", &MeshMeta::GenerateNormals) | //
           NReflHelper::AddField("GenerateSmoothNormals", &MeshMeta::GenerateSmoothNormals) | //
           NReflHelper::AddField("MergeDuplicateVertices", &MeshMeta::MergeDuplicateVertices) | //
           NReflHelper::AddField("RemoveUnusedMaterials", &MeshMeta::RemoveUnusedMaterials) | //
           NReflHelper::AddField("CanBeRemoved", &MeshMeta::CanBeRemoved) | //
           NReflHelper::AddField("import_scale", &MeshMeta::ImportScale);
}
