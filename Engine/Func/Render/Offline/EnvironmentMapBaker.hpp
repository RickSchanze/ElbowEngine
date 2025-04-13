//
// Created by Echo on 2025/4/12.
//

#pragma once
#include "Core/Math/Types.hpp"
#include "Resource/Assets/Material/Material.hpp"
#include "Resource/Assets/Texture/Texture2D.hpp"


class Material;
namespace RHI {
    class CommandBuffer;
}
class Texture2D;
class EnvironmentMapBaker {
public:
    /**
     * 烘焙环境贴图的辐照度图(相当于漫反射)
     * @param environment_sphere
     * @param output_size 输出IrradianceMap的尺寸
     * @param sample_delta 采样间隔 值越小 采样次数越多 越慢
     * @param intensity 强度
     */
    // TODO: BakeIrradianceMapAsync
    static Texture2D *BakeIrradianceMap(Texture2D *environment_sphere, Vector2f output_size, Float sample_delta = 0.01f, Float intensity = 1.0f);
    static void BakeIrradianceMap(Texture2D *environment_sphere, Texture2D *target, Float sample_delta = 0.01f, Float intensity = 1.0f);
    static void BakeIrradianceMap(RHI::CommandBuffer &buffer, Texture2D *environment_sphere, Texture2D *target, Material *material);

    /**
     * 烘焙环境贴图的预过滤环境贴图(Prefiltered Environment Map)(相当于高光)
     * @param buffer
     * @param environment_sphere
     * @param target
     * @param material
     */
    static void BakePrefilteredEnvironmentMap(Texture2D *environment_sphere, Texture2D *target, Material *material);

    /**
     * 烘焙环境贴图的预过滤环境贴图(Prefiltered Environment Map)(相当于高光)
     * @param buffer
     * @param environment_sphere
     * @param width 生成的预过滤贴图宽度
     * @param height 生产的预过滤贴图高度
     * @param mip_level 生成的预过滤贴图mip等级
     * @return 一个新的Texture2D*
     */
    static Texture2D *BakePrefilteredEnvironmentMap(Texture2D *environment_sphere, UInt32 width, UInt32 height, UInt32 mip_level = 1);

    static Texture2D* BakeIntegrateBRDFLookUpMap(UInt32 InSize);
    static void BakeIntegrateBRDFLookUpMap(const Texture2D *OutBRDFMap, Material *InMaterial);
};
