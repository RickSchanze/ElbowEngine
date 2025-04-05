//
// Created by Echo on 2025/3/23.
//
#pragma once


#include "Core/Core.hpp"

REFLECTED(SQLTable = "Mesh")
struct MeshMeta {
    REFLECTED_STRUCT(MeshMeta)

    [[nodiscard]] Int32 GetId() const { return id; }
    [[nodiscard]] Int32 GetObjectHandle() const { return object_handle; }
    [[nodiscard]] bool GetTriangulate() const { return triangulate; }
    [[nodiscard]] bool GetGenerateNormals() const { return generate_normals; }
    [[nodiscard]] bool GetGenerateSmoothNormals() const { return generate_smooth_normals; }
    [[nodiscard]] bool GetMergeDuplicateVertices() const { return merge_duplicate_vertices; }
    [[nodiscard]] bool GetRemoveUnusedMaterials() const { return remove_unused_materials; }
    [[nodiscard]] StringView GetPath() const { return path; }

    REFLECTED(SQLAttr = "(PrimaryKey, AutoIncrement)")
    Int32 id = 0;

    REFLECTED()
    Int32 object_handle = 0;

    REFLECTED()
    String path;

    Float import_scale = 1.0f;

    // 将所有网格多边形面转换为三角形面
    REFLECTED(Label = "三角化", Category = "基础")
    bool triangulate = true;

    REFLECTED(Label = "生成法线", Category = "基础")
    bool generate_normals = true;

    REFLECTED(Label = "生产平滑法线", Category = "基础")
    bool generate_smooth_normals = false;

    REFLECTED(Label = "合并重复顶点", Category = "基础")
    bool merge_duplicate_vertices = true;

    REFLECTED(Label = "移除多余材质", Category = "基础")
    bool remove_unused_materials = true;

    REFLECTED(Hidden)
    bool can_be_removed = true;

    // TODO: 一些高级选项
};

IMPL_REFLECTED_INPLACE(MeshMeta) {
    return Type::Create<MeshMeta>("MeshMeta") | refl_helper::Attribute(Type::ValueAttribute::SQLTable, "Mesh") |
           refl_helper::AddField("id", &MeshMeta::id).Attribute(Field::ValueAttribute::SQLAttr, "(PrimaryKey, AutoIncrement)") |
           refl_helper::AddField("object_handle", &MeshMeta::object_handle) | refl_helper::AddField("path", &MeshMeta::path) |
           refl_helper::AddField("triangulate", &MeshMeta::triangulate) | refl_helper::AddField("generate_normals", &MeshMeta::generate_normals) |
           refl_helper::AddField("generate_smooth_normals", &MeshMeta::generate_smooth_normals) |
           refl_helper::AddField("merge_duplicate_vertices", &MeshMeta::merge_duplicate_vertices) |
           refl_helper::AddField("remove_unused_materials", &MeshMeta::remove_unused_materials) |
           refl_helper::AddField("can_be_removed", &MeshMeta::can_be_removed) | refl_helper::AddField("import_scale", &MeshMeta::import_scale);
}
