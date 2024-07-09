/**
 * @file GLFWWindow.cpp
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#include "GLFWWindow.h"
#include "GameObject/GameObject.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "Input/Input.h"
#include "Path/Path.h"
#include "RHI/Vulkan/Render/Framebuffer.h"
#include "RHI/Vulkan/VulkanContext.h"
#include "Utils/StringUtils.h"
#include "vulkan/vk_enum_string_helper.h"

PLATFORM_WINDOW_NAMESPACE_BEGIN

class ImGuiRenderPass : public RHI::Vulkan::RenderPass
{

protected:
    void OnCreateAttachments() override
    {
        RHI::Vulkan::RenderPassAttachmentParam Param(vk::ImageUsageFlagBits::eSampled);
        Param.InitialLayout   = vk::ImageLayout::ePresentSrcKHR;
        Param.FinialLayout    = vk::ImageLayout::ePresentSrcKHR;
        Param.ReferenceLayout = vk::ImageLayout::eColorAttachmentOptimal;
        Param.StoreOp         = vk::AttachmentStoreOp::eStore;
        Param.LoadOp          = vk::AttachmentLoadOp::eDontCare;
        NewAttachment(Param, true);
    }

    void CreateSubpassDescription() override;
};

class ImGuiGraphicsPipeline : public IGraphicsPipeline
{
public:
    explicit ImGuiGraphicsPipeline(Ref<RHI::Vulkan::VulkanContext> InConext);

    void Initialize();
    void Finialize();

    ~ImGuiGraphicsPipeline() override;

protected:
    void CreateDescriptorPool();
    void CreateCommandBuffers();

public:
    void SubmitGraphicsQueue(
        int CurrentImageIndex, vk::Queue InGraphicsQueue, TArray<vk::Semaphore> InWaitSemaphores,
        TArray<vk::Semaphore> InSingalSemaphores, vk::Fence InFrameFence
    ) override;

    void Rebuild() override;

private:
    Ref<RHI::Vulkan::VulkanContext> mContext;

    vk::DescriptorPool                   mDescriptorPool = nullptr;
    TUniquePtr<RHI::Vulkan::CommandPool> mCommandPool;
    RHI::Vulkan::RenderPass*             mRenderPass = nullptr;
    TArray<vk::CommandBuffer>            mCommandBuffers;
};

void GLFWWindowSurface::Initialize()
{
    if (mAttachedInstanceHandle->IsValid())
    {
        VkSurfaceKHR   Surface{};
        const VkResult Result =
            glfwCreateWindowSurface(mAttachedInstanceHandle->GetHandle(), mWindow, nullptr, &Surface);
        if (Result != VK_SUCCESS)
        {
            String ErrorStr = StringUtils::FromAnsiString(string_VkResult(Result));
            throw VulkanException(std::format(L"创建窗口表面失败: {}", ErrorStr));
        }
        mSurfaceHandle = vk::SurfaceKHR(Surface);
    }
}

void ImGuiRenderPass::CreateSubpassDescription()
{
    RenderPass::CreateSubpassDescription();
    mDependency.srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    mDependency.dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    mDependency.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    mDependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
}

ImGuiGraphicsPipeline::ImGuiGraphicsPipeline(const Ref<RHI::Vulkan::VulkanContext> InConext) : mContext(InConext)
{
    Initialize();
    mContext.get().AddPipelineToRender(this);
}

void ImGuiGraphicsPipeline::Initialize()
{
    if (mDescriptorPool != nullptr) return;
    ImGui_ImplVulkan_InitInfo Info{};
    Info.Instance       = mContext.get().GetVulkanInstance()->GetHandle();
    Info.PhysicalDevice = mContext.get().GetPhysicalDevice()->GetHandle();
    Info.Device         = mContext.get().GetLogicalDevice()->GetHandle();
    Info.QueueFamily    = mContext.get().GetPhysicalDevice()->FindQueueFamilyIndices().graphics_family.value();
    Info.Queue          = mContext.get().GetLogicalDevice()->GetGraphicsQueue();
    Info.PipelineCache  = nullptr;
    Info.DescriptorPool = nullptr;   // 后面进行填充
    Info.Subpass        = 0;
    Info.MinImageCount  = mContext.get().GetSwapChainImageCount();
    Info.ImageCount     = mContext.get().GetSwapChainImageCount();
    Info.MSAASamples    = VK_SAMPLE_COUNT_1_BIT;

    CreateDescriptorPool();
    Info.DescriptorPool = mDescriptorPool;

    mCommandPool = RHI::Vulkan::CommandPool::CreateUnique(
        mContext.get().GetLogicalDevice(), vk::CommandPoolCreateFlagBits::eResetCommandBuffer
    );

    mRenderPass = new ImGuiRenderPass();
    mRenderPass->Initialize();

    // CommandBuffers
    vk::CommandBufferAllocateInfo AllocInfo = {};
    AllocInfo.setCommandPool(mContext.get().GetCommandPool()->GetHandle())
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandBufferCount(mContext.get().GetSwapChainImageCount());

    mCommandBuffers = mContext.get().GetLogicalDevice()->GetHandle().allocateCommandBuffers(AllocInfo);

    // 初始化Imgui
    ImGui_ImplVulkan_LoadFunctions(
        [](const char* Name, void* UserData) {
            return glfwGetInstanceProcAddress(static_cast<VkInstance>(UserData), Name);
        },
        mContext.get().GetVulkanInstance()->GetHandle()
    );
    ImGui_ImplVulkan_Init(&Info, mRenderPass->GetHandle());
    // 字体纹理
    CreateCommandBuffers();
}

void ImGuiGraphicsPipeline::Finialize()
{
    if (mDescriptorPool == nullptr) return;
    mCommandPool->Finialize();
    delete mRenderPass;
    mRenderPass = nullptr;
    ImGui_ImplVulkan_Shutdown();
    mContext.get().GetLogicalDevice()->GetHandle().destroyDescriptorPool(mDescriptorPool);
    mDescriptorPool = nullptr;
}

ImGuiGraphicsPipeline::~ImGuiGraphicsPipeline()
{
    Finialize();
    mContext.get().RemovePipelineFromRender(this);
}

void ImGuiGraphicsPipeline::CreateCommandBuffers()
{
    vk::CommandBufferAllocateInfo AllocInfo = {};
    AllocInfo.level                         = vk::CommandBufferLevel::ePrimary;
    AllocInfo.commandPool                   = mCommandPool->GetHandle();
    AllocInfo.commandBufferCount            = static_cast<uint32_t>(mRenderPass->GetFrameBuffers().size());
    mCommandBuffers = mContext.get().GetLogicalDevice()->GetHandle().allocateCommandBuffers(AllocInfo);
}

void ImGuiGraphicsPipeline::SubmitGraphicsQueue(
    int CurrentImageIndex, vk::Queue InGraphicsQueue, TArray<vk::Semaphore> InWaitSemaphores,
    TArray<vk::Semaphore> InSingalSemaphores, vk::Fence InFrameFence
)
{
    vk::CommandBufferBeginInfo begin_info = {};
    begin_info.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    mCommandBuffers[CurrentImageIndex].begin(&begin_info);
    vk::RenderPassBeginInfo         render_pass_info = {};
    TStaticArray<vk::ClearValue, 1> clear_values    = {};
    render_pass_info.renderPass                      = mRenderPass->GetHandle();
    render_pass_info.framebuffer                     = mRenderPass->GetFrameBuffer(CurrentImageIndex)->GetHandle();
    render_pass_info.renderArea                      = vk::Rect2D{{0, 0}, mContext.get().GetSwapChainExtent()};
    render_pass_info.clearValueCount                 = static_cast<uint32_t>(clear_values.size());
    render_pass_info.pClearValues                    = clear_values.data();
    mCommandBuffers[CurrentImageIndex].beginRenderPass(render_pass_info, vk::SubpassContents::eInline);
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), mCommandBuffers[CurrentImageIndex]);
    mCommandBuffers[CurrentImageIndex].endRenderPass();
    mCommandBuffers[CurrentImageIndex].end();
    vk::PipelineStageFlags WaitFlag = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    vk::SubmitInfo         Info     = {};
    Info.setSignalSemaphores(InSingalSemaphores)
        .setWaitSemaphores(InWaitSemaphores)
        .setCommandBuffers(mCommandBuffers[CurrentImageIndex])
        .setWaitDstStageMask(WaitFlag);
    InGraphicsQueue.submit(Info);
}

void ImGuiGraphicsPipeline::Rebuild()
{
    mRenderPass->Rebuild(false);
}

void ImGuiGraphicsPipeline::CreateDescriptorPool()
{
    vk::DescriptorPoolSize       PoolSizes[] = {{vk::DescriptorType::eCombinedImageSampler, 1}};
    vk::DescriptorPoolCreateInfo PoolCreateInfo;
    PoolCreateInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet).setMaxSets(1).setPoolSizes(PoolSizes);
    mDescriptorPool = mContext.get().GetLogicalDevice()->GetHandle().createDescriptorPool(PoolCreateInfo);
}

TUniquePtr<GLFWWindowSurface> GlfwWindow::GetWindowSurface()
{
    auto Surface = MakeUnique<GLFWWindowSurface>(nullptr, window_handle_);
    return Surface;
}

TArray<const char*> GlfwWindow::GetRequiredExtensions() const
{
    TArray<const char*> Extensions;
    uint32_t            Count = 0;
    const char**        Names = glfwGetRequiredInstanceExtensions(&Count);
    for (uint32_t i = 0; i < Count; ++i)
    {
        Extensions.emplace_back(Names[i]);
    }
    return Extensions;
}

Size2D GlfwWindow::GetWindowSize()
{
    glfwGetFramebufferSize(window_handle_, &width_, &height_);
    return {width_, height_};
}

void GlfwWindow::InitImGui(Ref<RHI::Vulkan::VulkanContext> InContext)
{
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(window_handle_, true);
    imgui_graphics_pipeline_ = new ImGuiGraphicsPipeline(InContext);
    SetupImGuiFonts();
}

void GlfwWindow::SetupImGuiFonts()
{
    ImGuiIO&                 IO                 = ImGui::GetIO();
    Path                     DefaultFontPath    = L"Fonts/Maple_UI.ttf";
    AnsiString               DefaultFontPathStr = DefaultFontPath.ToAnsiString();
    ImVector<ImWchar>        Ranges;
    ImFontGlyphRangesBuilder Builder;
    Builder.AddRanges(IO.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    Builder.AddText(U8("帧引擎"));
    Builder.BuildRanges(&Ranges);
    IO.Fonts->AddFontFromFileTTF(DefaultFontPathStr.c_str(), 30, nullptr, Ranges.Data);
    ImGui_ImplVulkan_CreateFontsTexture();
}

void GlfwWindow::ShutdownImGui()
{
    if (imgui_graphics_pipeline_)
    {
        // 代表ImGui已经初始化
        imgui_graphics_pipeline_->Finialize();
        ImGui_ImplGlfw_Shutdown();
    }
}

void GlfwWindow::BeginImGuiFrame()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GlfwWindow::EndImGuiFrame()
{
    // ImGui::Render();
}

void GlfwWindow::Initialize()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    const AnsiString Title = StringUtils::ToAnsiString(window_title_);
    window_handle_          = glfwCreateWindow(width_, height_, Title.c_str(), nullptr, nullptr);

    camera_object_ = New<Function::GameObject>(L"摄像机", nullptr);
    camera_object_->BeginPlay();
    camera_object_->AddComponent<Function::Camera>();
    New<Function::GameObject>(L"Dummy对象", camera_object_);
    New<Function::GameObject>(L"对象2");
    New<Function::GameObject>(L"对象3", New<Function::GameObject>(L"对象4", New<Function::GameObject>(L"对象5")));
}

void GlfwWindow::Finalize()
{
    if (imgui_graphics_pipeline_)
    {
        delete imgui_graphics_pipeline_;
        imgui_graphics_pipeline_ = nullptr;
    }
    glfwDestroyWindow(window_handle_);
    window_handle_ = nullptr;
    glfwTerminate();
}

void GlfwWindow::Tick(float DeltaTime)
{
    camera_object_->Tick(DeltaTime);
    glfwPollEvents();
    Input::InternalTick();
}

void GlfwWindow::SetMouseVisible(const bool InVisible) const
{
    glfwSetInputMode(window_handle_, GLFW_CURSOR, InVisible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

PLATFORM_WINDOW_NAMESPACE_END
