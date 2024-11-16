/**
 * @file SkySphereShader.cpp
 * @author Echo 
 * @Date 24-8-18
 * @brief 
 */

#include "SkySphereShader.h"

#include "Core/Math/MathTypes.h"

namespace function {

void SkySphereVertShader::RegisterShaderVariables()
{
    REGISTER_SHADER_VAR_BEGIN(0)
    struct UBOView
    {
        Matrix4x4 proj;
        Matrix4x4 view;
    };
    REGISTER_VERT_SHADER_VAR_AUTO_StaticUniformBuffer("ubo_view", sizeof(UBOView), false);
    REGISTER_SHADER_VAR_END()
}

void SkySphereFragShader::RegisterShaderVariables()
{
    REGISTER_SHADER_VAR_BEGIN(1)
    REGISTER_FRAG_SHADER_VAR_AUTO_Sampler2D("sky", false);
    REGISTER_SHADER_VAR_END()
}

}
