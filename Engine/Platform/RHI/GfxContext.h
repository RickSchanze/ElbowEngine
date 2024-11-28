/**
 * @file GfxContext.h
 * @author Echo 
 * @Date 24-11-19
 * @brief 
 */

#pragma once
#include "Core/Core.h"
#include "Core/Event/Event.h"
#include "Core/Math/MathTypes.h"
#include "Enums.h"

#include <cstdint>

namespace platform::rhi
{
DECLARE_EVENT(PostProcessVulkanExtensionsEvent, core::Array<core::String>, const core::Array<core::String>&);

inline PostProcessVulkanExtensionsEvent Event_PostProcessVulkanExtensions;

struct SurfaceFormat
{
    Format     format      = Format::Count;
    ColorSpace color_space = ColorSpace::Count;
};

struct SwapChainSupportInfo
{
    core::Array<SurfaceFormat> formats;
    core::Array<PresentMode>   present_modes;
    uint32_t                   min_image_count        = 0;
    uint32_t                   max_image_count        = 0;
    core::Size2D               current_extent         = {};
    core::Size2D               min_image_extent       = {};
    core::Size2D               max_image_extent       = {};
    uint32_t                   max_image_array_layers = 0;
};

struct PhysicalDeviceFeature
{
    // 各向异性过滤
    bool sampler_anisotropy = false;
};

struct PhysicalDeviceInfo
{
    core::String name;
    struct Limits
    {
        /** SampleCount */ int framebuffer_color_sample_count;
        /** SampleCount */ int framebuffer_depth_sample_count;
    };
    Limits limits;
};

class GfxContext
{
public:
    virtual ~GfxContext() = default;

    [[nodiscard]] virtual GraphicsAPI GetAPI() const = 0;

    [[nodiscard]] uint8_t GetSwapchainImageCount() const;

    /**
     * 查询交换链特性支持情况
     * @return
     */
    [[nodiscard]] virtual const SwapChainSupportInfo& QuerySwapChainSupportInfo() = 0;

    [[nodiscard]] virtual const PhysicalDeviceFeature& QueryDeviceFeature() = 0;

    /**
     * 查询设备基本信息
     * @return
     */
    [[nodiscard]] virtual const PhysicalDeviceInfo& QueryDeviceInfo() = 0;

    /**
     * 获取默认的深度/模版图像Format
     * @return
     */
    [[nodiscard]] virtual Format GetDefaultDepthStencilFormat() const = 0;

    /**
     * 获取默认Color Format(也就是交换链图像格式)
     * @return
     */
    [[nodiscard]] virtual Format GetDefaultColorFormat() const = 0;
};

GfxContext* GetGfxContext();
GfxContext& GetGfxContextRef();

/**
 * 设置使用的图形API并以此创建GfxContext
 * @param api 图形API
 */
void UseGraphicsAPI(GraphicsAPI api);

/**
 * 释放GfxContext
 */
void ReleaseGfxContext();

}   // namespace platform::rhi
