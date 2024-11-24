/**
 * @file Enums_Vulkan.cpp
 * @author Echo 
 * @Date 24-11-23
 * @brief 
 */

#include "Enums_Vulkan.h"
VkFormat RHIFormatToVkFormat(const platform::rhi::Format format)
{
    switch (format)
    {
    case platform::rhi::Format::R32G32B32_Float: return VK_FORMAT_R32G32B32_SFLOAT;
    case platform::rhi::Format::R16G16B16A16_UNorm: return VK_FORMAT_R16G16B16A16_UNORM;
    case platform::rhi::Format::R32G32_UInt: return VK_FORMAT_R32G32_UINT;
    case platform::rhi::Format::R8G8B8A8_UNorm: return VK_FORMAT_R8G8B8A8_UNORM;
    case platform::rhi::Format::R8G8B8A8_SNorm: return VK_FORMAT_R8G8B8A8_SNORM;
    case platform::rhi::Format::R8G8B8A8_UInt: return VK_FORMAT_R8G8B8A8_UINT;
    case platform::rhi::Format::R8G8B8A8_SInt: return VK_FORMAT_R8G8B8A8_SINT;
    case platform::rhi::Format::D32_Float_S8X24_UInt: return VK_FORMAT_D32_SFLOAT_S8_UINT;
    case platform::rhi::Format::D32_Float: return VK_FORMAT_D32_SFLOAT;
    case platform::rhi::Format::D24_UNorm_S8_UInt: return VK_FORMAT_D24_UNORM_S8_UINT;
    case platform::rhi::Format::D16_UNorm: return VK_FORMAT_D16_UNORM;
    case platform::rhi::Format::B8G8R8A8_SRGB: return VK_FORMAT_B8G8R8A8_SRGB;
    case platform::rhi::Format::A2B10G10R10_UNorm: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
    case platform::rhi::Format::B8G8R8A8_UNorm: return VK_FORMAT_B8G8R8A8_UNORM;
    case platform::rhi::Format::R16G16B16A16_Float: return VK_FORMAT_R16G16B16A16_SFLOAT;
    default: return VK_FORMAT_UNDEFINED;
    }
}

platform::rhi::Format VkFormatToRHIFormat(VkFormat format)
{
    switch (format)
    {
    case VK_FORMAT_R32G32B32_SFLOAT: return platform::rhi::Format::R32G32B32_Float;
    case VK_FORMAT_R16G16B16A16_UNORM: return platform::rhi::Format::R16G16B16A16_UNorm;
    case VK_FORMAT_R32G32_UINT: return platform::rhi::Format::R32G32_UInt;
    case VK_FORMAT_R8G8B8A8_UNORM: return platform::rhi::Format::R8G8B8A8_UNorm;
    case VK_FORMAT_R8G8B8A8_SNORM: return platform::rhi::Format::R8G8B8A8_SNorm;
    case VK_FORMAT_R8G8B8A8_UINT: return platform::rhi::Format::R8G8B8A8_UInt;
    case VK_FORMAT_R8G8B8A8_SINT: return platform::rhi::Format::R8G8B8A8_SInt;
    case VK_FORMAT_D32_SFLOAT_S8_UINT: return platform::rhi::Format::D32_Float_S8X24_UInt;
    case VK_FORMAT_D32_SFLOAT: return platform::rhi::Format::D32_Float;
    case VK_FORMAT_D24_UNORM_S8_UINT: return platform::rhi::Format::D24_UNorm_S8_UInt;
    case VK_FORMAT_D16_UNORM: return platform::rhi::Format::D16_UNorm;
    case VK_FORMAT_B8G8R8A8_SRGB: return platform::rhi::Format::B8G8R8A8_SRGB;
    case VK_FORMAT_A2B10G10R10_UNORM_PACK32: return platform::rhi::Format::A2B10G10R10_UNorm;
    case VK_FORMAT_B8G8R8A8_UNORM: return platform::rhi::Format::B8G8R8A8_UNorm;
    case VK_FORMAT_R16G16B16A16_SFLOAT: return platform::rhi::Format::R16G16B16A16_Float;
    default: return platform::rhi::Format::Count;
    }
}
VkColorSpaceKHR RHIColorSpaceToVkColorSpace(platform::rhi::ColorSpace color_space)
{
    switch (color_space)
    {
    case platform::rhi::ColorSpace::sRGB: return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    case platform::rhi::ColorSpace::HDR10: return VK_COLOR_SPACE_HDR10_ST2084_EXT;
    case platform::rhi::ColorSpace::Count: return VK_COLOR_SPACE_MAX_ENUM_KHR;
    }
    return VK_COLOR_SPACE_MAX_ENUM_KHR;
}

platform::rhi::ColorSpace VkColorSpaceToRHIColorSpace(VkColorSpaceKHR color_space)
{
    switch (color_space)
    {
    case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR: return platform::rhi::ColorSpace::sRGB;
    case VK_COLOR_SPACE_HDR10_ST2084_EXT: return platform::rhi::ColorSpace::HDR10;
    default: return platform::rhi::ColorSpace::Count;
    }
}

VkPresentModeKHR RHIPresentModeToVkPresentMode(platform::rhi::PresentMode present_mode)
{
    switch (present_mode)
    {
    case platform::rhi::PresentMode::Immediate: return VK_PRESENT_MODE_IMMEDIATE_KHR;
    case platform::rhi::PresentMode::VSync: return VK_PRESENT_MODE_FIFO_KHR;
    case platform::rhi::PresentMode::TripleBuffer: return VK_PRESENT_MODE_MAILBOX_KHR;
    case platform::rhi::PresentMode::Count: return VK_PRESENT_MODE_MAX_ENUM_KHR;
    }
    return VK_PRESENT_MODE_MAX_ENUM_KHR;
}

platform::rhi::PresentMode VkPresentModeToRHIPresentMode(VkPresentModeKHR present_mode)
{
    switch (present_mode)
    {
    case VK_PRESENT_MODE_IMMEDIATE_KHR: return platform::rhi::PresentMode::Immediate;
    case VK_PRESENT_MODE_FIFO_KHR: return platform::rhi::PresentMode::VSync;
    case VK_PRESENT_MODE_MAILBOX_KHR: return platform::rhi::PresentMode::TripleBuffer;
    default: return platform::rhi::PresentMode::Count;
    }
}

VkSampleCountFlagBits RHISampleCountToVkSampleCount(platform::rhi::SampleCount sample_count)
{
    switch (sample_count)
    {
    case platform::rhi::SampleCount::SC_1: return VK_SAMPLE_COUNT_1_BIT;
    case platform::rhi::SampleCount::SC_2: return VK_SAMPLE_COUNT_2_BIT;
    case platform::rhi::SampleCount::SC_4: return VK_SAMPLE_COUNT_4_BIT;
    case platform::rhi::SampleCount::SC_8: return VK_SAMPLE_COUNT_8_BIT;
    case platform::rhi::SampleCount::SC_16: return VK_SAMPLE_COUNT_16_BIT;
    case platform::rhi::SampleCount::SC_32: return VK_SAMPLE_COUNT_32_BIT;
    case platform::rhi::SampleCount::SC_64: return VK_SAMPLE_COUNT_64_BIT;
    case platform::rhi::SampleCount::SC_Count: return VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
    }
    return VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
}

platform::rhi::SampleCount VkSampleCountToRHISampleCount(VkSampleCountFlagBits sample_count)
{
    switch (sample_count)
    {
    case VK_SAMPLE_COUNT_1_BIT: return platform::rhi::SampleCount::SC_1;
    case VK_SAMPLE_COUNT_2_BIT: return platform::rhi::SampleCount::SC_2;
    case VK_SAMPLE_COUNT_4_BIT: return platform::rhi::SampleCount::SC_4;
    case VK_SAMPLE_COUNT_8_BIT: return platform::rhi::SampleCount::SC_8;
    case VK_SAMPLE_COUNT_16_BIT: return platform::rhi::SampleCount::SC_16;
    case VK_SAMPLE_COUNT_32_BIT: return platform::rhi::SampleCount::SC_32;
    case VK_SAMPLE_COUNT_64_BIT: return platform::rhi::SampleCount::SC_64;
    case VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM: return platform::rhi::SampleCount::SC_Count;
    }
    return platform::rhi::SampleCount::SC_Count;
}
