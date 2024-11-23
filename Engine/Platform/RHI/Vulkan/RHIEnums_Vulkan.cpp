/**
 * @file RHIFormat_Vulkan.cpp
 * @author Echo 
 * @Date 24-11-23
 * @brief 
 */

#include "RHIEnums_Vulkan.h"
VkFormat RHIFormatToVkFormat(const platform::RHIFormat format)
{
    switch (format)
    {
    case platform::RHIFormat::R32G32B32_Float: return VK_FORMAT_R32G32B32_SFLOAT;
    case platform::RHIFormat::R16G16B16A16_UNorm: return VK_FORMAT_R16G16B16A16_UNORM;
    case platform::RHIFormat::R32G32_UInt: return VK_FORMAT_R32G32_UINT;
    case platform::RHIFormat::R8G8B8A8_UNorm: return VK_FORMAT_R8G8B8A8_UNORM;
    case platform::RHIFormat::R8G8B8A8_SNorm: return VK_FORMAT_R8G8B8A8_SNORM;
    case platform::RHIFormat::R8G8B8A8_UInt: return VK_FORMAT_R8G8B8A8_UINT;
    case platform::RHIFormat::R8G8B8A8_SInt: return VK_FORMAT_R8G8B8A8_SINT;
    case platform::RHIFormat::D32_Float_S8X24_UInt: return VK_FORMAT_D32_SFLOAT_S8_UINT;
    case platform::RHIFormat::D32_Float: return VK_FORMAT_D32_SFLOAT;
    case platform::RHIFormat::D24_UNorm_S8_UInt: return VK_FORMAT_D24_UNORM_S8_UINT;
    case platform::RHIFormat::D16_UNorm: return VK_FORMAT_D16_UNORM;
    default: return VK_FORMAT_UNDEFINED;
    }
}


platform::RHIFormat VkFormatToRHIFormat(VkFormat format)
{
    switch (format)
    {
    case VK_FORMAT_R32G32B32_SFLOAT: return platform::RHIFormat::R32G32B32_Float;
    case VK_FORMAT_R16G16B16A16_UNORM: return platform::RHIFormat::R16G16B16A16_UNorm;
    case VK_FORMAT_R32G32_UINT: return platform::RHIFormat::R32G32_UInt;
    case VK_FORMAT_R8G8B8A8_UNORM: return platform::RHIFormat::R8G8B8A8_UNorm;
    case VK_FORMAT_R8G8B8A8_SNORM: return platform::RHIFormat::R8G8B8A8_SNorm;
    case VK_FORMAT_R8G8B8A8_UINT: return platform::RHIFormat::R8G8B8A8_UInt;
    case VK_FORMAT_R8G8B8A8_SINT: return platform::RHIFormat::R8G8B8A8_SInt;
    case VK_FORMAT_D32_SFLOAT_S8_UINT: return platform::RHIFormat::D32_Float_S8X24_UInt;
    case VK_FORMAT_D32_SFLOAT: return platform::RHIFormat::D32_Float;
    case VK_FORMAT_D24_UNORM_S8_UINT: return platform::RHIFormat::D24_UNorm_S8_UInt;
    case VK_FORMAT_D16_UNORM: return platform::RHIFormat::D16_UNorm;
    default: return platform::RHIFormat::Count;
    }
}
VkColorSpaceKHR RHIColorSpaceToVkColorSpace(platform::RHIColorSpace color_space)
{
    switch (color_space)
    {
    case platform::RHIColorSpace::sRGB: return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    case platform::RHIColorSpace::HDR10: return VK_COLOR_SPACE_HDR10_ST2084_EXT;
    case platform::RHIColorSpace::Count: return VK_COLOR_SPACE_MAX_ENUM_KHR;
    }
    return VK_COLOR_SPACE_MAX_ENUM_KHR;
}

platform::RHIColorSpace VkColorSpaceToRHIColorSpace(VkColorSpaceKHR color_space)
{
    switch (color_space)
    {
    case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR: return platform::RHIColorSpace::sRGB;
    case VK_COLOR_SPACE_HDR10_ST2084_EXT: return platform::RHIColorSpace::HDR10;
    default: return platform::RHIColorSpace::Count;
    }
}

VkPresentModeKHR RHIPresentModeToVkPresentMode(platform::RHIPresentMode present_mode)
{
    switch (present_mode)
    {
    case platform::RHIPresentMode::Immediate: return VK_PRESENT_MODE_IMMEDIATE_KHR;
    case platform::RHIPresentMode::VSync: return VK_PRESENT_MODE_FIFO_KHR;
    case platform::RHIPresentMode::TripleBuffer: return VK_PRESENT_MODE_MAILBOX_KHR;
    case platform::RHIPresentMode::Count: return VK_PRESENT_MODE_MAX_ENUM_KHR;
    }
    return VK_PRESENT_MODE_MAX_ENUM_KHR;
}

platform::RHIPresentMode VkPresentModeToRHIPresentMode(VkPresentModeKHR present_mode)
{
    switch (present_mode)
    {
    case VK_PRESENT_MODE_IMMEDIATE_KHR: return platform::RHIPresentMode::Immediate;
    case VK_PRESENT_MODE_FIFO_KHR: return platform::RHIPresentMode::VSync;
    case VK_PRESENT_MODE_MAILBOX_KHR: return platform::RHIPresentMode::TripleBuffer;
    default: return platform::RHIPresentMode::Count;
    }
}

VkSampleCountFlagBits RHISampleCountToVkSampleCount(platform::RHISampleCount sample_count)
{
    switch (sample_count)
    {
    case platform::RHISampleCount::RHISC_1: return VK_SAMPLE_COUNT_1_BIT;
    case platform::RHISampleCount::RHISC_2: return VK_SAMPLE_COUNT_2_BIT;
    case platform::RHISampleCount::RHISC_4: return VK_SAMPLE_COUNT_4_BIT;
    case platform::RHISampleCount::RHISC_8: return VK_SAMPLE_COUNT_8_BIT;
    case platform::RHISampleCount::RHISC_16: return VK_SAMPLE_COUNT_16_BIT;
    case platform::RHISampleCount::RHISC_32: return VK_SAMPLE_COUNT_32_BIT;
    case platform::RHISampleCount::RHISC_64: return VK_SAMPLE_COUNT_64_BIT;
    case platform::RHISampleCount::Count: return VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
    }
    return VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
}

platform::RHISampleCount VkSampleCountToRHISampleCount(VkSampleCountFlagBits sample_count)
{
    switch (sample_count)
    {
    case VK_SAMPLE_COUNT_1_BIT: return platform::RHISampleCount::RHISC_1;
    case VK_SAMPLE_COUNT_2_BIT: return platform::RHISampleCount::RHISC_2;
    case VK_SAMPLE_COUNT_4_BIT: return platform::RHISampleCount::RHISC_4;
    case VK_SAMPLE_COUNT_8_BIT: return platform::RHISampleCount::RHISC_8;
    case VK_SAMPLE_COUNT_16_BIT: return platform::RHISampleCount::RHISC_16;
    case VK_SAMPLE_COUNT_32_BIT: return platform::RHISampleCount::RHISC_32;
    case VK_SAMPLE_COUNT_64_BIT: return platform::RHISampleCount::RHISC_64;
    case VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM: return platform::RHISampleCount::Count;
    }
    return platform::RHISampleCount::Count;
}
