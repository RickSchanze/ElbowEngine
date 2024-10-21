/**
 * @file TextureFormat.cpp
 * @author Echo 
 * @Date 24-10-20
 * @brief 
 */

#include "TextureFormat.h"

DXGI_FORMAT rhi::d3d12::GetTextureFormat(rhi::TextureFormat format)
{
    switch (format)
    {
    case TextureFormat::R32G32B32_Float: return DXGI_FORMAT_R32G32_FLOAT;
    case TextureFormat::R16G16B16A16_UNorm: return DXGI_FORMAT_R16G16B16A16_UNORM;
    case TextureFormat::R32G32_UInt: return DXGI_FORMAT_R32G32_UINT;
    case TextureFormat::R8G8B8A8_UNorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
    case TextureFormat::R8G8B8A8_SNorm: return DXGI_FORMAT_R8G8B8A8_SNORM;
    case TextureFormat::R8G8B8A8_SInt: return DXGI_FORMAT_R8G8B8A8_SINT;
    case TextureFormat::R8G8B8A8_UInt: return DXGI_FORMAT_R8G8B8A8_UINT;

    case TextureFormat::D32_Float: return DXGI_FORMAT_D32_FLOAT;
    case TextureFormat::D16_UNorm: return DXGI_FORMAT_D16_UNORM;
    case TextureFormat::D24_UNorm_S8_UInt: return DXGI_FORMAT_D24_UNORM_S8_UINT;
    case TextureFormat::D32_Float_S8X24_UInt: return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
    }
    return DXGI_FORMAT_UNKNOWN;
}
