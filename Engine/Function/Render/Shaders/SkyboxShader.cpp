/**
 * @file SkyboxShader.cpp
 * @author Echo 
 * @Date 24-8-24
 * @brief 
 */

#include "SkyboxShader.h"
#include "Math/MathTypes.h"

namespace function
{
void SkyboxVertShader::RegisterShaderVariables()
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

void SkyboxFragShader::RegisterShaderVariables()
{
    REGISTER_SHADER_VAR_BEGIN(1)
    REGISTER_FRAG_SHADER_VAR_AUTO_SmaplerCube("sky", false);
    REGISTER_SHADER_VAR_END()
}
}
