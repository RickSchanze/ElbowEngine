//
// Created by Echo on 2025/3/22.
//

#pragma once
#include "Core/Async/Exec/ExecFuture.hpp"
#include "Core/Manager/MManager.hpp"
#include "Core/Math/Types.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "Core/Misc/UniquePtr.hpp"
#include "Core/String/String.hpp"
#include "Enums.hpp"


class FrameAllocator;
namespace rhi {
    struct Fence;
    struct Semaphore;
    class CommandBuffer;
    class Buffer;
    struct BufferDesc;
    class CommandPool;
    struct CommandPoolCreateInfo;
    class GraphicsPipeline;
    struct GraphicsPipelineDesc;
    class LowShader;
    class RenderPass;
    class DescriptorSetLayout;
    struct DescriptorSetLayoutDesc;
    class DescriptorSetPool;
    struct DescriptorSetPoolDesc;
    class ImageView;
    class Image;
    struct ImageDesc;
    struct ImageViewDesc;
    class Sampler;
    struct SamplerDesc;

    struct SurfaceFormat {
        Format format = Format::Count;
        ColorSpace color_space = ColorSpace::Count;
    };

    struct SwapChainSupportInfo {
        Array<SurfaceFormat> formats;
        Array<PresentMode> present_modes;
        UInt32 min_image_count = 0;
        UInt32 max_image_count = 0;
        Vector2f current_extent = {};
        Vector2f min_image_extent = {};
        Vector2f max_image_extent = {};
        UInt32 max_image_array_layers = 0;
    };

    struct PhysicalDeviceFeature {
        // 各向异性过滤
        bool sampler_anisotropy = false;
    };

    struct PhysicalDeviceInfo {
        String name;
        struct Limits {
            /** SampleCount */ int framebuffer_color_sample_count;
            /** SampleCount */ int framebuffer_depth_sample_count;
        };
        Limits limits;
    };

    struct SubmitParameter {
        Fence *fence = nullptr;
        QueueFamilyType submit_queue_type = QueueFamilyType::Graphics;
        Semaphore *signal_semaphore = nullptr;
        UInt64 wait_value = 0;
        Semaphore *wait_semaphore = nullptr;
        UInt64 signal_value = 0;
    };

    class GfxContext {
    public:
        GfxContext();

        virtual ~GfxContext();

        virtual GraphicsAPI GetAPI() const = 0;

        UInt8 GetSwapchainImageCount() const;

        /**
         * 查询交换链特性支持情况
         * @return
         */
        virtual SwapChainSupportInfo QuerySwapChainSupportInfo() = 0;

        virtual const PhysicalDeviceFeature &QueryDeviceFeature() = 0;

        /**
         * 查询设备基本信息
         * @return
         */
        virtual const PhysicalDeviceInfo &QueryDeviceInfo() = 0;

        /**
         * 获取默认的深度/模版图像Format
         * @return
         */
        virtual Format GetDefaultDepthStencilFormat() const = 0;

        /**
         * 获取默认Color Format(不一定是交换链图像格式, HDR时不一样)
         * @return
         */
        virtual Format GetDefaultColorFormat() const = 0;

        /**
         * 提交命令到GPU
         * @param buffer
         * @param parameter
         */
        virtual exec::ExecFuture<> Submit(SharedPtr<CommandBuffer> buffer, const SubmitParameter &parameter) = 0;

        /**
         * 创建同步原语: Fence
         * @param signaled 创建出来的是否处于signaled状态
         * @return
         */
        virtual UniquePtr<Fence> CreateFence(bool signaled) = 0;

        /**
         * 创建一个Buffer
         * @return
         */
        virtual SharedPtr<Buffer> CreateBuffer(const BufferDesc &, StringView debug_name = "") = 0;

        /**
         * 创建一个CommandPool
         * @return
         */
        virtual SharedPtr<CommandPool> CreateCommandPool(const CommandPoolCreateInfo &create_info) = 0;

        /**
         * 获取用于传输队列的CommandPool
         * @return
         */
        virtual CommandPool &GetTransferPool() = 0;

        /**
         * 创建一个底层装饰器
         * @return
         */
        virtual SharedPtr<LowShader> CreateShader(const char *code, size_t size, StringView debug_name = "") = 0;

        /**
         * 创建一个GraphicsPipeline, 你需要自己管理它的生命周期
         * 对于render_pass,
         *   对于移动端设备, 由于其设备特性, render_pass和其subpass设计符合架构设计
         *   对于桌面端, 可以直接传入nullptr使用dynamic rendering(vulkan)而无需担心性能损失
         * @param create_info
         * @param render_pass
         * @return
         */
        virtual UniquePtr<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineDesc &create_info, RenderPass *render_pass) = 0;

        /**
         * 创建一个新的DescriptorSetLayout
         * 一般来说 你不需要直接调用这个接口,
         * 使用 DescriptorSetLayoutPool.GetOrCreate
         * 这是个单例
         * @param desc
         * @return
         */
        virtual SharedPtr<DescriptorSetLayout> CreateDescriptorSetLayout(const DescriptorSetLayoutDesc &desc) = 0;

        /**
         * 同步获取当前交换链图像索引
         * @param signal_semaphore 信号量(非Timeline Semaphore)
         * @return
         */
        virtual Optional<Int32> GetCurrentSwapChainImageIndexSync(Semaphore *signal_semaphore = nullptr) = 0;

        /**
         * 创建一个信号量
         * 为什么叫CreateASemaphore? 因为byd微软把CreateSemaphore定义成宏了
         * @param init_value 初始值
         * @param vk_timeline 是否是timeline semaphore(vulkan)
         * @return
         */
        virtual UniquePtr<Semaphore> Create_Semaphore(UInt64 init_value = 0, bool vk_timeline = true) = 0;

        /**
         * 进行Present操作
         * @param image_index
         * @param wait_semaphore
         * @return
         */
        virtual bool Present(UInt32 image_index, Semaphore *wait_semaphore = nullptr) = 0;

        /**
         * 获取交换链图像视图
         * @param index
         * @return
         */
        virtual ImageView *GetSwapChainView(UInt32 index) = 0;

        /**
         * 获取交换链图像
         * @param index
         * @return
         */
        virtual Image *GetSwapChainImage(UInt32 index) = 0;

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
        virtual SharedPtr<DescriptorSetPool> CreateDescriptorSetPool(const DescriptorSetPoolDesc &desc) = 0;

        /**
         * 创建一个Image
         * @param desc
         * @param debug_name
         * @return
         */
        virtual SharedPtr<Image> CreateImage(const ImageDesc &desc, StringView debug_name = "") = 0;

        virtual SharedPtr<ImageView> CreateImageView(const ImageViewDesc &desc, StringView debug_name = "") = 0;

        SharedPtr<Sampler> GetSampler(const SamplerDesc &desc);

        virtual void Update();

        FrameAllocator &GetCommandAllocator();

    protected:
        virtual SharedPtr<Sampler> CreateSampler(const SamplerDesc &desc, StringView debug_name = "") = 0;

    private:
        static void PreDestroyed(GfxContext *self);

        FrameAllocator *cmd_allocator_ = nullptr;

        Map<UInt64, SharedPtr<Sampler>> sampler_pool_;
    };

    GfxContext *GetGfxContext();
    GfxContext &GetGfxContextRef();

    /**
     * 设置使用的图形API并以此创建GfxContext
     * @param api 图形API
     */
    void UseGraphicsAPI(GraphicsAPI api);

    /**
     * 释放GfxContext
     */
    void ReleaseGfxContext();

    /**
     * 用于代理GfxContext的Manager 只用于销毁
     */
    class GfxContextLifeTimeProxyManager : public Manager<GfxContextLifeTimeProxyManager> {
    public:
        Float GetLevel() const override { return 6; }
        StringView GetName() const override { return "GfxContextProxyManager"; }

        void Shutdown() override { ReleaseGfxContext(); }
    };
} // namespace rhi
