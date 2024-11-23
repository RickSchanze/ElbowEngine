/**
 * @file RHIEnums.h
 * @author Echo 
 * @Date 24-11-23
 * @brief 
 */

#pragma once
#include "Core/Core.h"

namespace platform
{

enum class ENUM() RHIFormat
{
    // 每个元素由3个32位浮点数分量构成
    R32G32B32_Float,
    // 每个元素由4个16位分量构成, 每个分量被映射到[0, 1]
    R16G16B16A16_UNorm,
    // 每个元素由2个32位无符号整数构成
    R32G32_UInt,
    // 每个元素由4个8位无符号数构成, 被映射到[0, 1]
    R8G8B8A8_UNorm,
    // 每个元素由4个8位有符号数构成, 被映射到[-1, 1]
    R8G8B8A8_SNorm,
    // 每个元素由4个8位无符号数构成, 被映射到[0, 255]
    R8G8B8A8_UInt,
    // 每个元素由4个8位有符号数构成, 被映射到[-128, 127]
    R8G8B8A8_SInt,
    // 深度/模版: 64位 32位给深度, 8位给模版([0, 255]), 24位用作对齐
    D32_Float_S8X24_UInt,
    // 深度: 32位浮点数
    D32_Float,
    // 深度/模版: 24位给深度([0, 1]), 8位给模版([0, 255])
    D24_UNorm_S8_UInt,
    // 深度: 16位([0, 1])
    D16_UNorm,
    // 超出范围
    Count,
};

enum class ENUM() RHIColorSpace
{
    // 非线性sRGB, 适用于大部分普通内容, 标准显示器
    sRGB,
    // HDR10
    HDR10,
    // 超出范围
    Count,
};

enum class ENUM() RHIPresentMode
{
    // 垂直同步
    VSync,
    // 立即刷新
    Immediate,
    // 三重缓冲
    TripleBuffer,
    // 超出范围
    Count,
};

enum class ENUM() GraphicsAPI
{
    Vulkan,
    D3D12,
    OpenGL,
    Null,
    Count,
};

/**
 * MSAA采样次数
 * Vulkan API指示这里是个Flag Enum, Follow it.
 */
enum ENUM(Flag) RHISampleCount
{
    // 1次
    RHISC_1  = 0b0000001,
    // 2次
    RHISC_2  = 0b0000010,
    // 4次
    RHISC_4  = 0b0000100,
    // 8次
    RHISC_8  = 0b0001000,
    // 16次
    RHISC_16 = 0b0010000,
    // 32次
    RHISC_32 = 0b0100000,
    // 64次
    RHISC_64 = 0b1000000,
    // 超出范围
    Count,
};

}
