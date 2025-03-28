//
// Created by Echo on 2025/3/22.
//

#pragma once
#include "IResource.hpp"

namespace rhi {
/**
 * Surface是对Vulkan Surface的抽象
 * 交由platform::Window类实现并创建
 */
class Surface : public IResource
{
};
}
