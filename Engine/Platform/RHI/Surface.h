/**
 * @file Surface.h
 * @author Echo 
 * @Date 24-11-23
 * @brief 
 */

#pragma once
#include "IResource.h"

namespace platform::rhi
{
/**
 * Surface是对Vulkan Surface的抽象
 * 交由platform::Window类实现并创建
 */
class Surface : public IResource
{
};
}
