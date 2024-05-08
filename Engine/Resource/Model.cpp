/**
 * @file Model.cpp
 * @author Echo 
 * @Date 24-5-8
 * @brief 
 */

#include "Model.h"

#include "CoreGlobal.h"
#include "tiny_obj_loader.h"

RESOURCE_NAMESPACE_BEGIN
Model::Model(const Path& InModelPath) : mPath(InModelPath) {
    auto ModelPath = InModelPath.ToAnsiString();
    if (!mPath.IsExist()) {
        LOG_ERROR_CATEGORY(Vulkan, L"模型文件{}不存在", mPath.ToString());
        return;
    }

    tinyobj::attrib_t                Attrib;
    std::vector<tinyobj::shape_t>    Shapes;
    std::vector<tinyobj::material_t> Materials;
    std::string                      Warn;
    std::string                      Err;
    if (!LoadObj(&Attrib, &Shapes, &Materials, &Warn, &Err, ModelPath.c_str())) {
        throw std::runtime_error(Err);
    }
    std::unordered_map<Vertex, uint32_t> UniqueVertices = {};
    for (const auto& Shape: Shapes) {
        for (const auto& Index: Shape.mesh.indices) {
            Vertex Vertex = {};
            Vertex.Pos    = {
                Attrib.vertices[3 * Index.vertex_index + 0],
                Attrib.vertices[3 * Index.vertex_index + 1],
                Attrib.vertices[3 * Index.vertex_index + 2]
            };
            // 由于obj格式默认纹理坐标原点在左下角 而Vulkan在左上角 因此需要转换
            Vertex.UV = {
                Attrib.texcoords[2 * Index.texcoord_index + 0],
                1 - Attrib.texcoords[2 * Index.texcoord_index + 1],
            };
            if (!UniqueVertices.contains(Vertex)) {
                UniqueVertices[Vertex] = static_cast<uint32_t>(mVertices.size());
                mVertices.push_back(Vertex);
            }
            mVertices.push_back(Vertex);
            mIndices.push_back(UniqueVertices[Vertex]);
        }
    }
}
RESOURCE_NAMESPACE_END
