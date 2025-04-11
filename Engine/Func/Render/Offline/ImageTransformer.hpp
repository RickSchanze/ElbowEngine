//
// Created by Echo on 2025/4/9.
//

#pragma once
#include "Core/TypeAlias.hpp"
#include "Resource/Assets/Texture/Texture2D.hpp"


class Material;
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
     * @param environment_sphere
     * @param size
     * @param cube_format
     */
    static Texture2D *CalculateIrradianceMap(Texture2D *environment_sphere);
    static void CalculateIrradianceMap(Texture2D *environment_sphere, Texture2D *target);
    static void CalculateIrradianceMap(rhi::CommandBuffer& buffer, Texture2D *environment_sphere, Texture2D *target);
    static void CalculateIrradianceMap(rhi::CommandBuffer& buffer, Texture2D *environment_sphere, Texture2D *target, Material* material);
};
