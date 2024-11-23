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
#include "RHIEnums.h"

#include <cstdint>

namespace platform::rhi
{
DECLARE_EVENT(PostProcessVulkanExtensionsEvent, core::Array<core::String>, const core::Array<core::String>&);

inline PostProcessVulkanExtensionsEvent Event_PostProcessVulkanExtensions;

struct SurfaceFormat
{
    RHIFormat     format;
    RHIColorSpace color_space;
};

struct SwapChainSupportInfo
{
    core::Array<SurfaceFormat>  formats;
    core::Array<RHIPresentMode> present_modes;
    uint32_t                    min_image_count        = 0;
    uint32_t                    max_image_count        = 0;
    core::Size2D                current_extent         = {};
    core::Size2D                min_image_extent       = {};
    core::Size2D                max_image_extent       = {};
    uint32_t                    max_image_array_layers = 0;
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
        /** RHISampleCount */ int framebuffer_color_sample_count;
        /** RHISampleCount */ int framebuffer_depth_sample_count;
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
    [[nodiscard]] virtual SwapChainSupportInfo QuerySwapChainSupportInfo() = 0;

    [[nodiscard]] virtual PhysicalDeviceFeature QueryDeviceFeature() = 0;

    /**
     * 查询设备基本信息
     * @return
     */
    [[nodiscard]] virtual PhysicalDeviceInfo QueryDeviceInfo() = 0;
};

GfxContext& GetGfxContext();

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
