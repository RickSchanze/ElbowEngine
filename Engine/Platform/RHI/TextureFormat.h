/**
 * @file TextureFormat.h
 * @author Echo 
 * @Date 24-10-20
 * @brief 
 */

#pragma once

#include "Core/Base/Base.h"

namespace rhi
{
enum class TextureFormat
{
    R32G32B32_Float,      // 3个32位浮点数
    R16G16B16A16_UNorm,   // 4个16位分量, 每个分量被映射至[0,1]
    R32G32_UInt,          // 2个32位无符号整数
    R8G8B8A8_UNorm,       // 4个8位分量, 每个分量被映射至[0,1]
    R8G8B8A8_SNorm,       // 4个8位分量, 每个分量被映射至[-1,1], 并应用sRGB颜色空间
    R8G8B8A8_SInt,        // 4个8位分量, 每个分量被映射至[-128,127]
    R8G8B8A8_UInt,        // 4个8位分量, 每个分量被映射至[0,255]

    // 深度缓冲可用格式
    D32_Float_S8X24_UInt,   // 32位浮点数深度, 8位整数模板, 剩下24为保留
    D32_Float,              // 32位浮点数深度
    D24_UNorm_S8_UInt,      // 24位无符号整数深度, 8位整数模板
    D16_UNorm,              // 16位无符号整数深度
};
}   // namespace rhi

#define _TEXTURE_FORMAT_CASE(format) \
    case rhi::TextureFormat::format: return #format

template<>
inline core::StringView GetEnumString<rhi::TextureFormat>(rhi::TextureFormat format)
{
    switch (format)
    {
        _TEXTURE_FORMAT_CASE(R32G32B32_Float);
        _TEXTURE_FORMAT_CASE(R16G16B16A16_UNorm);
        _TEXTURE_FORMAT_CASE(R32G32_UInt);
        _TEXTURE_FORMAT_CASE(R8G8B8A8_UNorm);
        _TEXTURE_FORMAT_CASE(R8G8B8A8_SNorm);
        _TEXTURE_FORMAT_CASE(R8G8B8A8_SInt);
        _TEXTURE_FORMAT_CASE(R8G8B8A8_UInt);
        _TEXTURE_FORMAT_CASE(D32_Float_S8X24_UInt);
        _TEXTURE_FORMAT_CASE(D32_Float);
        _TEXTURE_FORMAT_CASE(D24_UNorm_S8_UInt);
        _TEXTURE_FORMAT_CASE(D16_UNorm);
    }
    return "Unknown TextureFormat";
}
