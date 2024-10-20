/**
 * @file TextureFormat.cpp
 * @author Echo 
 * @Date 24-10-20
 * @brief 
 */

#include "TextureFormat.h"

vk::Format rhi::vulkan::GetTextureFormat(TextureFormat format)
{
    switch (format)
    {
    case TextureFormat::R32G32_UInt: return vk::Format::eR32G32Uint;
    case TextureFormat::R32G32B32_Float: return vk::Format::eR32G32B32Sfloat;
    case TextureFormat::R8G8B8A8_SInt: return vk::Format::eR8G8B8A8Sint;
    case TextureFormat::R8G8B8A8_SNorm: return vk::Format::eR8G8B8A8Snorm;
    case TextureFormat::R8G8B8A8_UInt: return vk::Format::eR8G8B8A8Uint;
    case TextureFormat::R8G8B8A8_UNorm: return vk::Format::eR8G8B8A8Uint;
    case TextureFormat::R16G16B16A16_UNorm: return vk::Format::eR16G16B16A16Snorm;

    case TextureFormat::D32_Float: return vk::Format::eD32Sfloat;
    case TextureFormat::D16_UNorm: return vk::Format::eD16Unorm;
    case TextureFormat::D24_UNorm_S8_UInt: return vk::Format::eD24UnormS8Uint;
    case TextureFormat::D32_Float_S8X24_UInt: return vk::Format::eD32SfloatS8Uint;
    }
    return vk::Format::eUndefined;
}
