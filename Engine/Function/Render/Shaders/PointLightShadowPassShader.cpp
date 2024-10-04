/**
 * @file PointLightShadowPassShader.cpp
 * @author Echo 
 * @Date 24-8-11
 * @brief 
 */

#include "PointLightShadowPassShader.h"

#include "Math/MathTypes.h"

void Function::PointLightShadowPassVertShader::RegisterShaderVariables()
{
    REGISTER_SHADER_VAR_BEGIN(0);
    REGISTER_VERT_SHADER_VAR_AUTO_StaticUniformBuffer("ubo_view", 2 * sizeof(Matrix4x4), false);
    REGISTER_VERT_SHADER_VAR_AUTO_DynamicUniformBuffer(
        "ubo_instance", sizeof(glm::mat4) * g_engine_statistics.graphics.max_dynamic_model_uniform_buffer_count, sizeof(glm::mat4), false
    );
    REGISTER_PUSH_CONSTANT("camera_view", 0, rhi::vulkan::EShaderStage::Vertex, sizeof(Matrix4x4));
    REGISTER_SHADER_VAR_END()
}

void Function::PointLightShadowPassFragShader::RegisterShaderVariables() {}
