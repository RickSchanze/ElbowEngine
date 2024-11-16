/**
 * @file InfiniteGridShader.cpp
 * @author Echo 
 * @Date 24-9-22
 * @brief 
 */

#include "InfiniteGridShader.h"

#include "Core/Math/MathTypes.h"

namespace function
{
void InfiniteGridVertShader::RegisterShaderVariables()
{
    REGISTER_SHADER_VAR_BEGIN(0);
    REGISTER_VERT_SHADER_VAR_AUTO_StaticUniformBuffer("ubo_view", 3 * sizeof(Matrix4x4), false);
    REGISTER_SHADER_VAR_END()
}

void InfiniteGridFragShader::RegisterShaderVariables() {}
}   // namespace function
