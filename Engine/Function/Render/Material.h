/**
 * @file Material.h
 * @author Echo 
 * @Date 24-7-26
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "FunctionCommon.h"

namespace RHI::Vulkan
{
class GraphicsPipeline;
}
namespace RHI::Vulkan
{
class ShaderProgram;
}
namespace RHI::Vulkan
{
class Shader;
}
FUNCTION_NAMESPACE_BEGIN

/**
 * 材质的封装
 * 作用：Shader参数的集合
 *      通过此类设置Shader参数
 */
class Material {
public:


private:
    RHI::Vulkan::ShaderProgram* shader_program_ = nullptr;
    RHI::Vulkan::GraphicsPipeline* pipeline_ = nullptr;

    // TMap<AnsiString, >

    bool dirty_ = false;
};

FUNCTION_NAMESPACE_END
