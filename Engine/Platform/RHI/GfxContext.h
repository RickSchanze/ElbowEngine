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
#include "Core/Memory/FrameAllocator.h"
#include "Enums.h"
#include "Pipeline.h"
#include "SyncPrimitives.h"

#include <cstdint>

namespace platform::rhi
{
class Image;
}
namespace platform::rhi
{
class ImageView;
}
// clang-format off
namespace platform::rhi
{
struct DescriptorSetLayoutDesc;
class  RenderPass;
class  DescriptorSetLayout;
class  GraphicsPipeline;
class  LowShader;
struct CommandPoolCreateInfo;
class  CommandPool;
struct Fence;
class  CommandBuffer;
}   // namespace platform::rhi
// clang-format on
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
    Semaphore*      signal_semaphore  = nullptr;
    uint64_t        wait_value        = 0;
    Semaphore*      wait_semaphore    = nullptr;
    uint64_t        signal_value      = 0;
};

constexpr auto MEMORY_POOL_ID_CMD = 2;

class GfxContext
{
public:
    GfxContext();

    virtual ~GfxContext() = default;

    [[nodiscard]] virtual GraphicsAPI GetAPI() const = 0;

    [[nodiscard]] uint8_t GetSwapchainImageCount() const;

    /**
     * 查询交换链特性支持情况
     * @return
     */
    [[nodiscard]] virtual SwapChainSupportInfo QuerySwapChainSupportInfo() = 0;

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
    virtual core::exec::AsyncResultHandle<> Submit(core::SharedPtr<CommandBuffer> buffer, const SubmitParameter& parameter) = 0;

    /**
     * 创建同步原语: Fence
     * @param signaled 创建出来的是否处于signaled状态
     * @return
     */
    [[nodiscard]] virtual core::UniquePtr<Fence> CreateFence(bool signaled) = 0;

    /**
     * 创建一个Buffer
     * @return
     */
    [[nodiscard]] virtual core::SharedPtr<Buffer> CreateBuffer(const BufferDesc&, core::StringView debug_name = "") = 0;

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
    [[nodiscard]] virtual core::SharedPtr<LowShader> CreateShader(const char* code, size_t size, core::StringView debug_name = "") = 0;

    /**
     * 创建一个GraphicsPipeline, 你需要自己管理它的生命周期
     * 对于render_pass,
     *   对于移动端设备, 由于其设备特性, render_pass和其subpass设计符合架构设计
     *   对于桌面端, 可以直接传入nullptr使用dynamic rendering(vulkan)而无需担心性能损失
     * @param create_info
     * @param render_pass
     * @return
     */
    [[nodiscard]] virtual core::UniquePtr<GraphicsPipeline>
    CreateGraphicsPipeline(const GraphicsPipelineDesc& create_info, RenderPass* render_pass) = 0;

    /**
     * 创建一个新的DescriptorSetLayout
     * 一般来说 你不需要直接调用这个接口,
     * 使用 DescriptorSetLayoutPool.GetOrCreate
     * 这是个单例
     * @param desc
     * @return
     */
    [[nodiscard]] virtual core::SharedPtr<DescriptorSetLayout> CreateDescriptorSetLayout(const DescriptorSetLayoutDesc& desc) = 0;

    /**
     * 同步获取当前交换链图像索引
     * @param signal_semaphore 信号量(非Timeline Semaphore)
     * @return
     */
    [[nodiscard]] virtual core::Optional<int32_t> GetCurrentSwapChainImageIndexSync(Semaphore* signal_semaphore = nullptr) = 0;

    /**
     * 创建一个信号量
     * 为什么叫CreateASemaphore? 因为byd微软把CreateSemaphore定义成宏了
     * @param init_value 初始值
     * @param vk_timeline 是否是timeline semaphore(vulkan)
     * @return
     */
    [[nodiscard]] virtual core::UniquePtr<Semaphore> CreateASemaphore(uint64_t init_value = 0, bool vk_timeline = true) = 0;

    /**
     * 进行Present操作
     * @param image_index
     * @param wait_semaphore
     * @return
     */
    virtual bool Present(uint32_t image_index, Semaphore* wait_semaphore = nullptr) = 0;

    /**
     * 获取交换链图像视图
     * @param index
     * @return
     */
    virtual ImageView* GetSwapChainView(UInt32 index) = 0;

    /**
     * 获取交换链图像
     * @param index
     * @return
     */
    virtual Image* GetSwapChainImage(UInt32 index) = 0;

    /**
     * 等待队列执行完成
     * !!慢速操作!!
     */
    virtual void WaitForQueueExecution(QueueFamilyType type) = 0;

    /**
     * 等待设备空闲
     * !! 慢速操作 !!
     */
    virtual void WaitForDeviceIdle() = 0;

    /**
     * 重新调整交换链大小
     * @param width
     * @param height
     */
    virtual void ResizeSwapChain(Int32 width, Int32 height) = 0;

    /**
     * 创建一个DescriptorSetPool
     * @param desc
     * @return
     */
    virtual core::SharedPtr<DescriptorSetPool> CreateDescriptorSetPool(const DescriptorSetPoolDesc& desc) = 0;

    virtual void Update();

    core::FrameAllocator& GetCommandAllocator();

private:
    core::FrameAllocator* cmd_allocator_ = nullptr;
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

/**
 * 用于代理GfxContext的Manager 只用于销毁
 */
class GfxContextLifeTimeProxyManager : public core::Manager<GfxContextLifeTimeProxyManager>
{
public:
    core::ManagerLevel GetLevel() const override { return core::ManagerLevel::L4; }
    core::StringView GetName() const override { return "GfxContextProxyManager"; }

    void Shutdown() override {ReleaseGfxContext();}
};

}   // namespace platform::rhi
