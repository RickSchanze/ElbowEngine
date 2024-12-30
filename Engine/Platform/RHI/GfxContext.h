/**
 * @file GfxContext.h
 * @author Echo 
 * @Date 24-11-19
 * @brief 
 */

#pragma once
#include "Buffer.h"
#include "Core/Async/Execution/StartAsync.h"
#include "Core/Core.h"
#include "Core/Event/Event.h"
#include "Core/Math/MathTypes.h"
#include "Enums.h"
#include "Pipeline.h"

#include <cstdint>

namespace platform::rhi
{
class GraphicsPipeline;
}
namespace platform::rhi
{
class LowShader;
}
namespace platform::rhi
{
struct CommandPoolCreateInfo;
class CommandPool;
struct Fence;
class CommandBuffer;
}   // namespace platform::rhi
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

class RHIException : public core::Exception
{
public:
    explicit RHIException(const core::String& msg) : Exception(core::String::Format("RHI错误:\n{}", msg)) {}
};

struct SubmitParameter
{
    Fence*          fence             = nullptr;
    QueueFamilyType submit_queue_type = QueueFamilyType::Graphics;
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

    /**
     * 提交命令到GPU
     * @param buffer
     * @param parameter
     */
    virtual core::exec::AsyncResultHandle Submit(CommandBuffer& buffer, const SubmitParameter& parameter) = 0;

    /**
     * 创建同步原语: Fence
     * @return
     */
    [[nodiscard]] virtual core::SharedPtr<Fence> CreateFence() = 0;

    /**
     * 创建一个Buffer
     * @param create_info
     * @return
     */
    [[nodiscard]] virtual core::SharedPtr<Buffer> CreateBuffer(const BufferCreateInfo& create_info) = 0;

    /**
     * 创建一个CommandPool
     * @return
     */
    [[nodiscard]] virtual core::SharedPtr<CommandPool> CreateCommandPool(const CommandPoolCreateInfo& create_info) = 0;

    /**
     * 获取用于传输队列的CommandPool
     * @return
     */
    [[nodiscard]] virtual CommandPool& GetTransferPool() = 0;

    /**
     * 创建一个底层装饰器
     * @return
     */
    [[nodiscard]] virtual core::SharedPtr<LowShader> CreateShader(const char* code, size_t size) = 0;


    [[nodiscard]] virtual core::UniquePtr<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineDesc& create_info) = 0;
};

GfxContext* GetGfxContext();
GfxContext& GetGfxContextRef();

/**
 * 设置使用的图形API并以此创建GfxContext
 * @param api 图形API
 */
void UseGraphicsAPI(GraphicsAPI api);

DECLARE_MULTICAST_EVENT_NO_PARAMS(GfxContextPreInitializedEvent);
DECLARE_MULTICAST_EVENT(GfxContextPostInitializedEvent, GfxContext*);
DECLARE_MULTICAST_EVENT(GfxContextPreDestroyedEvent, GfxContext*);
DECLARE_MULTICAST_EVENT_NO_PARAMS(GfxContextPostDestroyedEvent);

inline GfxContextPreInitializedEvent  Event_GfxContextPreInitialized;
inline GfxContextPostInitializedEvent Event_GfxContextPostInitialized;
inline GfxContextPreDestroyedEvent    Event_GfxContextPreDestroyed;
inline GfxContextPostDestroyedEvent   Event_GfxContextPostDestroyed;

/**
 * 释放GfxContext
 */
void ReleaseGfxContext();

}   // namespace platform::rhi
