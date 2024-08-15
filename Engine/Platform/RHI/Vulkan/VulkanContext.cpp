/**
 * @file VulkanContext.cpp
 * @author Echo
 * @Date 24-4-23
 * @brief
 */

#include "VulkanContext.h"

#include "CoreEvents.h"
#include "CoreGlobal.h"
#include "Instance.h"
#include "Render/CommandPool.h"
#include "Render/GraphicsPipeline.h"
#include "Utils/ContainerUtils.h"
#include "Utils/StringUtils.h"

RHI_VULKAN_NAMESPACE_BEGIN

VulkanContext::~VulkanContext()
{
    if (IsValid())
    {
        Finalize();
    }
}

TUniquePtr<VulkanContext> VulkanContext::CreateUnique(const TSharedPtr<Instance>& instance)
{
    auto Rtn = MakeUnique<VulkanContext>(Protected{}, instance);
    return Rtn;
}

VulkanContext::VulkanContext(Protected, const TSharedPtr<Instance>& instance)
{
    if (s_context_ == nullptr)
    {
        s_context_ = this;
    }
    else
    {
        LOG_INFO_CATEGORY(Vulkan, L"多次初始化VulkanContext,忽略本次初始化调用");
        return;
    }
    renderer_id_ = s_renderer_id_count_++;
    LOG_INFO_CATEGORY(Vulkan, L"创建Vulkan渲染器[id = {}]中", renderer_id_, g_engine_statistics.graphics.swapchain_image_count);
    vulkan_instance_      = instance;
    physical_device_      = vulkan_instance_->PickPhysicalDevice();
    const auto Properties = physical_device_->GetProperties();
    auto       Name       = StringUtils::FromAnsiString(Properties.deviceName);
    LOG_INFO_CATEGORY(Vulkan, L"物理设备选择完成. 选用: {}", Name);
    logical_device_ = physical_device_->CreateLogicalDeviceUnique();
    swap_chain_     = logical_device_->CreateSwapChain(g_engine_statistics.graphics.swapchain_image_count, 1920, 1080);
    // 初始化命令生产者
    command_pool_   = CommandPool::CreateUnique(logical_device_, vk::CommandPoolCreateFlagBits::eResetCommandBuffer, "ApplicationCommandPool");
    CreateSyncObjecs();
    CreateCommandBuffers();
    Initialize();
}

VulkanContext* VulkanContext::Get()
{
    if (s_context_ == nullptr)
    {
        throw VulkanException(L"VulkanContext未初始化");
    }
    return s_context_;
}

void VulkanContext::Initialize()
{
    LOG_INFO_CATEGORY(Vulkan, L"Vulkan渲染器[id = {}]创建完成", renderer_id_, g_engine_statistics.graphics.swapchain_image_count);
    // 一般来说这个应该是第一个
    // @TODO: 具有优先级的队列系统
    OnAppWindowResized.Add(&VulkanContext::RebuildSwapChain);
}

void VulkanContext::Finalize()
{
    if (!IsValid()) return;
    DestroyCommandBuffers();
    // 清理所有的Sampler
    Sampler::DestroyAllSamplers();
    CleanSyncObjects();
    command_pool_->Finialize();
    // 当前GraphicsPipeline创建时自己加载文件因此有可能抛出异常，此时mGraphicsPipeline为nullptr
    // 在调用就成了未定义行为，因此加一个if判断
    // TODO: 将Shader文件读取操作放在GraphicsPipeline之外
    swap_chain_->Finialize(true);
    logical_device_->Finialize();
    LOG_INFO_CATEGORY(Vulkan, L"Vukan渲染器[id = {}]清理完成", renderer_id_);
}

vk::Semaphore VulkanContext::SubmitGraphicsQueue(GraphicsQueueSubmitParams submit_params, vk::Fence fence_to_trigger)
{
    vk::Queue         graphics_queue = logical_device_->GetGraphicsQueue();
    vk::CommandBuffer cb             = GetCurrentCommandBuffer();
    vk::SubmitInfo    submit_info;

    vk::Semaphore rtn_semaphore = nullptr;

    submit_info.setCommandBuffers(cb);
    submit_info.setWaitSemaphores(submit_params.semaphores_to_wait);
    if (submit_params.has_self_semaphore)
    {
        // 如果这次提交需要自己生成一个信号量触发
        constexpr vk::SemaphoreCreateInfo create_info;
        rtn_semaphore = logical_device_->CreateDeviceSemaphore(create_info);
        submit_params.semaphores_to_singal.push_back(rtn_semaphore);
        all_wait_semaphores_.push_back(rtn_semaphore);
    }
    submit_info.setSignalSemaphores(submit_params.semaphores_to_singal);
    submit_info.pWaitDstStageMask = submit_params.wait_stages.data();
    graphics_queue.submit(submit_info, fence_to_trigger);
    return rtn_semaphore;
}

void VulkanContext::PrepareFrameRender()
{
    // 等待当前帧的指令缓冲结束执行
    const TStaticArray fences      = {in_flight_fences_[g_engine_statistics.current_frame_index]};
    vk::Result         wait_result = logical_device_->WaitForFences(fences);
    if (wait_result != vk::Result::eSuccess)
    {
        LOG_CRITIAL_CATEGORY(Vulkan.Render, L"等待指令缓冲执行失败");
        return;
    }
    // 获取可用图像索引
    VkResult acquire_result = vkAcquireNextImageKHR(
        logical_device_->GetHandle(),
        swap_chain_->GetHandle(),
        UINT64_MAX,
        image_available_semaphores_[g_engine_statistics.current_frame_index],
        nullptr,
        &g_engine_statistics.current_image_index
    );

    if (acquire_result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        // 交换链过期，重建交换链
        // TODO: 加上交换链过期事件
        return;
    }
    if (acquire_result != VK_SUCCESS && acquire_result != VK_SUBOPTIMAL_KHR)
    {
        LOG_CRITIAL_CATEGORY(Vulkan.Render, L"无法获取交换链图像");
        return;
    }
    // 重置当前帧的Fence
    logical_device_->ResetFences(fences);
}

void VulkanContext::PostFrameRender()
{
    // 呈现
    vk::PresentInfoKHR PresentInfo = {};
    TStaticArray       SwapChains  = {swap_chain_->GetHandle()};
    // TODO: 这里需要等待的信号量需要再看看
    PresentInfo
        .setSwapchains(SwapChains)                                  //
        .setImageIndices(g_engine_statistics.current_image_index)   //
        .setWaitSemaphores(all_wait_semaphores_);                   //

    const auto Result = logical_device_->GetPresentQueue().presentKHR(&PresentInfo);

    if (Result == vk::Result::eErrorOutOfDateKHR || Result == vk::Result::eSuboptimalKHR)
    {
        int w, h;
        OnGetAppWindowSize.Broadcast(&w, &h);
        RebuildSwapChain(w, h);
    }
    else if (Result != vk::Result::eSuccess)
    {
        throw VulkanException(std::format(L"呈现交换链图像失败: {}", StringUtils::FromAnsiString(to_string(Result))));
    }

    logical_device_->GetPresentQueue().waitIdle();

    for (auto& semaphore: all_wait_semaphores_)
    {
        logical_device_->DestroySemaphore(semaphore);
    }
    all_wait_semaphores_.clear();

    g_engine_statistics.IncreaseFrameIndex();
}

bool VulkanContext::IsValid() const
{
    // clang-format off
    return
        swap_chain_ && swap_chain_->IsValid() &&
        command_pool_ &&
        logical_device_ && logical_device_->IsValid() &&
        physical_device_ && physical_device_->IsValid() &&
        vulkan_instance_ && vulkan_instance_->IsValid();
    // clang-format on
}

void VulkanContext::RebuildSwapChain(int w, int h)
{
    Get()->wait_swapchain_rebuild_ = true;
    if (w == 0 || h == 0)
    {
        OnAppNeedWait.Broadcast();
        return;
    }
    Get()->swap_chain_->Finialize(false);
    Get()->swap_chain_                     = Get()->logical_device_->CreateSwapChain(g_engine_statistics.graphics.swapchain_image_count, w, h, false);
    g_engine_statistics.window_size.width  = w;
    g_engine_statistics.window_size.height = h;
    Get()->wait_swapchain_rebuild_         = false;
}

vk::Format VulkanContext::GetDepthImageFormat() const
{
    return physical_device_->FindSupportFormat(
        {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
}

uint32_t VulkanContext::GetMinUniformBufferOffsetAlignment() const
{
    return physical_device_->GetProperties().limits.minUniformBufferOffsetAlignment;
}

vk::CommandBuffer VulkanContext::GetCurrentCommandBuffer() const
{
    return command_buffers_[g_engine_statistics.current_image_index];
}

vk::CommandBuffer VulkanContext::BeginRecordCommandBuffer()
{
    vk::CommandBuffer cb = GetCurrentCommandBuffer();
    cb.reset(vk::CommandBufferResetFlagBits::eReleaseResources);
    vk::CommandBufferBeginInfo begin_info;
    begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    cb.begin(begin_info);
    return cb;
}

void VulkanContext::EndRecordCommandBuffer()
{
    GetCurrentCommandBuffer().end();
}

bool VulkanContext::CanRender() const
{
    return !wait_swapchain_rebuild_;
}

void VulkanContext::CreateSyncObjecs()
{
    image_available_semaphores_.resize(g_engine_statistics.graphics.parallel_render_frame_count);
    in_flight_fences_.resize(g_engine_statistics.graphics.parallel_render_frame_count);

    vk::SemaphoreCreateInfo SemaphoreInfo = {};
    vk::FenceCreateInfo     FenceInfo     = {};
    FenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

    // 为多帧并行渲染创建信号量
    for (size_t i = 0; i < g_engine_statistics.graphics.parallel_render_frame_count; i++)
    {
        image_available_semaphores_[i] = logical_device_->GetHandle().createSemaphore(SemaphoreInfo);
        in_flight_fences_[i]           = logical_device_->GetHandle().createFence(FenceInfo);
    }
}

void VulkanContext::CleanSyncObjects() const
{
    for (size_t i = 0; i < g_engine_statistics.graphics.parallel_render_frame_count; i++)
    {
        logical_device_->GetHandle().destroySemaphore(image_available_semaphores_[i]);
        logical_device_->GetHandle().destroyFence(in_flight_fences_[i]);
    }
}

void VulkanContext::CreateCommandBuffers()
{
    vk::CommandBufferAllocateInfo alloc_info;
    alloc_info.level               = vk::CommandBufferLevel::ePrimary;
    alloc_info.commandPool         = command_pool_->GetHandle();
    alloc_info.commandBufferCount  = g_engine_statistics.graphics.swapchain_image_count;
    AnsiString command_buffer_name = "GlobalCommandBuffer";
    command_buffers_               = command_pool_->CreateCommandBuffers(alloc_info, command_buffer_name.c_str(), &command_buffers_names_);
}

void VulkanContext::DestroyCommandBuffers(){
    command_pool_->DestroyCommandBuffers(command_buffers_);
    command_buffers_.clear();
    command_buffers_names_.clear();
}



RHI_VULKAN_NAMESPACE_END
