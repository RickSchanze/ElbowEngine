//
// Created by Echo on 24-12-25.
//

#pragma once
#include "IResource.h"

namespace platform::rhi {

enum class ShaderStage
{
    Vertex,
    Fragment,
    Compute,
    Count,
};

/**
 * LowLevel Shader
 */
class LowShader : public IResource
{

};

}
