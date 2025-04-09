//
// Created by Echo on 2025/4/9.
//

#pragma once
#include "Core/TypeAlias.hpp"


namespace rhi {
    class CommandBuffer;
}
namespace rhi {
    enum class Format;
}
class Texture2D;
class ImageTransformer {
public:
    /**
     * 将球面贴图转换为立方体贴图
     * @param tex
     * @param size
     * @param cube_format
     */
    static Texture2D* TransformSphereMapToCubeMap(Texture2D* tex, Float size, rhi::Format cube_format);
};
