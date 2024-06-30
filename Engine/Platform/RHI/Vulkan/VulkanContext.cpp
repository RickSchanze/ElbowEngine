/**
 * @file VulkanContext.cpp
 * @author Echo
 * @Date 24-4-23
 * @brief
 */

#include "VulkanContext.h"

#include "../../../Tool/EngineApplication.h"
#include "CoreGlobal.h"
#include "Instance.h"
#include "Render/CommandPool.h"
#include "Render/GraphicsPipeline.h"
#include "Utils/ContainerUtils.h"
#include "Utils/StringUtils.h"

#include <iostream>

RHI_VULKAN_NAMESPACE_BEGIN

VulkanContext::~VulkanContext()
{
    if (IsValid())
    {
        Finalize();
    }
}

TUniquePtr<VulkanContext> VulkanContext::CreateUnique(const TSharedPtr<Instance> &InVulkanInstance)
{
    auto Rtn = MakeUnique<VulkanContext>(Protected{}, InVulkanInstance);
    Rtn->CreateGraphicsPipeline();
    return Rtn;
}

VulkanContext::VulkanContext(Protected, const TSharedPtr<Instance> &InVulkanInstance)
{
    if (sContext == nullptr)
    {
        sContext = this;
    }
    else
    {
        LOG_INFO_CATEGORY(Vulkan, L"多次初始化VulkanContext,忽略本次初始化调用");
        return;
    }
    renderer_id_ = s_renderer_id_count_++;
    LOG_INFO_CATEGORY(Vulkan, L"创建Vulkan渲染器[id = {}]中", renderer_id_, swap_chain_image_count_);
    vulkan_instance_ = InVulkanInstance;
    physical_device_ = vulkan_instance_->PickPhysicalDevice();
    const auto Properties = physical_device_->GetProperties();
    auto Name = StringUtils::FromAnsiString(Properties.deviceName);
    LOG_INFO_CATEGORY(Vulkan, L"物理设备选择完成. 选用: {}", Name);
    logical_device_ = physical_device_->CreateLogicalDeviceUnique();
    swap_chain_ = logical_device_->CreateSwapChain(swap_chain_image_count_, 1920, 1080);
    // 初始化命令生产者
    command_pool_ = CommandPool::CreateUnique(logical_device_);
    CreateSyncObjecs();
    Initialize();
}

VulkanContext &VulkanContext::Get()
{
    if (sContext == nullptr)
    {
        throw VulkanException(L"VulkanContext未初始化");
    }
    return *sContext;
}

void VulkanContext::Initialize()
{
    LOG_INFO_CATEGORY(Vulkan, L"Vulkan渲染器[id = {}]创建完成", renderer_id_, swap_chain_image_count_);
    g_engine_statistics.swapchain_image_count = GetSwapChainImageCount();
}

void VulkanContext::Finalize()
{
    if (!IsValid())
        return;
    CleanSyncObjects();
    command_pool_->Finialize();
    // 当前GraphicsPipeline创建时自己加载文件因此有可能抛出异常，此时mGraphicsPipeline为nullptr
    // 在调用就成了未定义行为，因此加一个if判断
    // TODO: 将Shader文件读取操作放在GraphicsPipeline之外
    if (graphics_pipeline_)
    {
        delete graphics_pipeline_;
        graphics_pipeline_ = nullptr;
    }
    swap_chain_->Finialize();
    logical_device_->Finialize();
    LOG_INFO_CATEGORY(Vulkan, L"Vukan渲染器[id = {}]清理完成", renderer_id_);
}

void VulkanContext::Draw()
{
    const vk::Device device = logical_device_->GetHandle();
    // 首先等待当前帧的指令缓冲结束执行
    const auto current_frame_index = g_engine_statistics.current_frame_index;

    const TStaticArray<vk::Fence, 1> fences = {in_flight_fences_[current_frame_index]};
    auto _ = device.waitForFences(fences, VK_TRUE, UINT64_MAX);
    // 获取可用图像索引
    if (VkResult acquire_result = vkAcquireNextImageKHR(device, swap_chain_->GetHandle(), UINT64_MAX,
                                                        image_available_semaphores_[current_frame_index], nullptr,
                                                        &g_engine_statistics.current_image_index);

        acquire_result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        // 交换链已经过期，需要重建
        RebuildSwapChain();
        return;
    }
    else if (acquire_result != VK_SUCCESS && acquire_result != VK_SUBOPTIMAL_KHR)
    {
        throw VulkanException(L"无法获取交换链图像");
    }
    auto image_index = g_engine_statistics.current_image_index;
    device.resetFences(fences);
    // 这里更新UniformBuffer
    graphics_pipeline_->UpdateUniformBuffer(g_engine_statistics.current_image_index);

    TArray WaitSemaphores = {image_available_semaphores_[current_frame_index]};
    TArray<vk::Semaphore> SingalSemaphores = {};

    constexpr vk::SemaphoreCreateInfo SemaphoreInfo = {};
    for (auto &Pipeline : render_graphics_pipelines_)
    {
        SingalSemaphores.push_back(device.createSemaphore(SemaphoreInfo));
    }

    for (int i = 0; i < render_graphics_pipelines_.size(); i++)
    {
        if (i == 0)
        {
            render_graphics_pipelines_[i]->SubmitGraphicsQueue(image_index, logical_device_->GetGraphicsQueue(),
                                                               WaitSemaphores, {SingalSemaphores[0]},
                                                               in_flight_fences_[current_frame_index]);
        }
        else
        {
            render_graphics_pipelines_[i]->SubmitGraphicsQueue(image_index, logical_device_->GetGraphicsQueue(),
                                                               {SingalSemaphores[0]}, {SingalSemaphores[1]},
                                                               in_flight_fences_[current_frame_index]);
        }
    }

    // 呈现
    vk::PresentInfoKHR PresentInfo = {};
    TStaticArray SwapChains = {swap_chain_->GetHandle()};
    PresentInfo.setWaitSemaphores(SingalSemaphores).setSwapchains(SwapChains).setImageIndices(image_index);

    const auto Result = logical_device_->GetPresentQueue().presentKHR(&PresentInfo);

    if (Result == vk::Result::eErrorOutOfDateKHR || Result == vk::Result::eSuboptimalKHR ||
        Tool::EngineApplication::Get().bFrameBufferResized)
    {
        RebuildSwapChain();
        Tool::EngineApplication::Get().bFrameBufferResized = false;
    }
    else if (Result != vk::Result::eSuccess)
    {
        throw VulkanException(std::format(L"呈现交换链图像失败: {}", StringUtils::FromAnsiString(to_string(Result))));
    }

    logical_device_->GetPresentQueue().waitIdle();

    for (auto &Semaphore : SingalSemaphores)
    {
        device.destroySemaphore(Semaphore);
    }

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

void VulkanContext::RebuildSwapChain()
{
    // TODO: 使用PipelineCache
    auto Size = Tool::EngineApplication::Get().GetWindowSize();
    while (Size.Width == 0 || Size.Height == 0)
    {
        Size = Tool::EngineApplication::Get().GetWindowSize();
        glfwWaitEvents();
    }
    logical_device_->GetGraphicsQueue().waitIdle();
    logical_device_->GetPresentQueue().waitIdle();
    logical_device_->GetHandle().waitIdle();

    swap_chain_->Finialize();
    swap_chain_ = logical_device_->CreateSwapChain(swap_chain_image_count_, Size.Width, Size.Height);

    for (const auto &Pipeline : render_graphics_pipelines_)
    {
        Pipeline->Rebuild();
    }
}

vk::Format VulkanContext::GetDepthImageFormat()
{
    return physical_device_->FindSupportFormat(
        {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint}, vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

void VulkanContext::CreateGraphicsPipeline()
{
    // 创建图形管线
    // 寻找深度图像格式
    PipelineInitializer Initializer{};
    Initializer.ShaderStage.FragmentShaderPath = L"Shaders/frag.spv";
    Initializer.ShaderStage.VertexShaderPath = L"Shaders/vert.spv";
    graphics_pipeline_ = new GraphicsPipeline(Initializer);
}

// void VulkanContext::AddPipelineToRender(IGraphicsPipeline* InPipeline) {
//     mRenderGraphicsPipelines.push_back(InPipeline);
// }
//
// void VulkanContext::RemovePipelineFromRender(IGraphicsPipeline* InPipeline) {
//     ContainerUtils::Remove(mRenderGraphicsPipelines, InPipeline);
// }

void VulkanContext::CreateSyncObjecs()
{
    image_available_semaphores_.resize(g_engine_statistics.parallel_render_frame_count);
    image_render_finished_semaphores_.resize(g_engine_statistics.parallel_render_frame_count);
    in_flight_fences_.resize(g_engine_statistics.parallel_render_frame_count);

    vk::SemaphoreCreateInfo SemaphoreInfo = {};
    vk::FenceCreateInfo FenceInfo = {};
    FenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

    // 为多帧并行渲染创建信号量
    for (size_t i = 0; i < g_engine_statistics.parallel_render_frame_count; i++)
    {
        image_available_semaphores_[i] = logical_device_->GetHandle().createSemaphore(SemaphoreInfo);
        image_render_finished_semaphores_[i] = logical_device_->GetHandle().createSemaphore(SemaphoreInfo);
        in_flight_fences_[i] = logical_device_->GetHandle().createFence(FenceInfo);
    }
}

void VulkanContext::CleanSyncObjects()
{
    for (size_t i = 0; i < g_engine_statistics.parallel_render_frame_count; i++)
    {
        logical_device_->GetHandle().destroySemaphore(image_available_semaphores_[i]);
        logical_device_->GetHandle().destroySemaphore(image_render_finished_semaphores_[i]);
        logical_device_->GetHandle().destroyFence(in_flight_fences_[i]);
    }
}

RHI_VULKAN_NAMESPACE_END
