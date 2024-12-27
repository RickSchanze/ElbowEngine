//
// Created by Echo on 24-12-16.
//

#pragma once
#include "Core/Core.h"

#include GEN_HEADER("Resource.MeshMeta.generated.h")

namespace resource
{
class CLASS(SQLTable = "Mesh") MeshMeta
{
    GENERATED_CLASS(MeshMeta)

public:
    [[nodiscard]] int32_t          GetId() const { return id; }
    [[nodiscard]] int32_t          GetObjectHandle() const { return object_handle; }
    [[nodiscard]] bool             GetTriangulate() const { return triangulate; }
    [[nodiscard]] bool             GetGenerateNormals() const { return generate_normals; }
    [[nodiscard]] bool             GetGenerateSmoothNormals() const { return generate_smooth_normals; }
    [[nodiscard]] bool             GetMergeDuplicateVertices() const { return merge_duplicate_vertices; }
    [[nodiscard]] bool             GetRemoveUnusedMaterials() const { return remove_unused_materials; }
    [[nodiscard]] core::StringView GetPath() const { return path; }

    PROPERTY(SQLAttr = "(PrimaryKey, AutoIncrement)")
    int32_t id = 0;

    PROPERTY()
    int32_t object_handle = 0;

    PROPERTY()
    core::String path;

    // 将所有网格多边形面转换为三角形面
    PROPERTY(Label = "三角化", Category = "基础")
    bool triangulate = true;

    PROPERTY(Label = "生成法线", Category = "基础")
    bool generate_normals = true;

    PROPERTY(Label = "生产平滑法线", Category = "基础")
    bool generate_smooth_normals = false;

    PROPERTY(Label = "合并重复顶点", Category = "基础")
    bool merge_duplicate_vertices = true;

    PROPERTY(Label = "移除多余材质", Category = "基础")
    bool remove_unused_materials = true;

    PROPERTY(Hidden)
    bool can_be_removed = true;

    // TODO: 一些高级选项
};
}   // namespace core::resource
