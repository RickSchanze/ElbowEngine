/**
 * @file Shader.h
 * @author Echo 
 * @Date 24-4-27
 * @brief 
 */

#pragma once

#include "Path/Path.h"
#include "vulkan/vulkan.hpp"

enum class EShaderType
{
    Vertex, Fragment, None
};

class Shader {

private:
    EShaderType mShaderType = EShaderType::None;
    Path mShaderPath;
    vk::ShaderModule mShaderModule;
};
