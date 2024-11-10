/**
 * @file Shader.cpp
 * @author Echo 
 * @Date 24-11-9
 * @brief 
 */

#include "Shader.h"
bool platform::rhi::Shader::IsValid() const
{
    return stage_ != ShaderStage::Count;
}
