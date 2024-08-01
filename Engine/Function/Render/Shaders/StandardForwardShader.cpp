/**
 * @file StandardForwardShader.cpp
 * @author Echo 
 * @Date 24-8-1
 * @brief 
 */

#include "StandardForwardShader.h"

#include <glm/glm.hpp>

FUNCTION_NAMESPACE_BEGIN

void StandardForwardVertShader::RegisterShaderVariables()
{
    REGISTER_SHADER_VAR_BEGIN(0)
    REGISTER_VERT_SHADER_VAR_AUTO_StaticUniformBuffer("ubo_view", 3 * sizeof(glm::mat4), 0);
    REGISTER_VERT_SHADER_VAR_AUTO_DynamicUniformBuffer(
        "ubo_instance", sizeof(glm::mat4) * g_engine_statistics.graphics.max_dynamic_model_uniform_buffer_count, sizeof(glm::mat4)
    );
    REGISTER_SHADER_VAR_END
}

void StandardForwardFragShader::RegisterShaderVariables()
{
    REGISTER_SHADER_VAR_BEGIN(2)
    REGISTER_FRAG_SHADER_VAR_AUTO_Sampler2D("texSampler");
    REGISTER_FRAG_SHADER_VAR_AUTO_StaticUniformBuffer("ubo_point_lights", 64 * g_engine_statistics.graphics.max_point_light_count);
    REGISTER_SHADER_VAR_END
}

FUNCTION_NAMESPACE_END
