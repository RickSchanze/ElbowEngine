/**
 * @file TextureFormat.h
 * @author Echo 
 * @Date 24-10-20
 * @brief 
 */

#pragma once

#include "RHI/TextureFormat.h"
// 这里头问价包含顺序不能变, 因为d3d12.h会导致rttr无法编译通过
#include "d3d12.h"
namespace rhi::d3d12
{
DXGI_FORMAT        GetTextureFormat(rhi::TextureFormat format);
inline DXGI_FORMAT f;
}   // namespace rhi::d3d12
