/**
 * @file GLFWWindow.cpp
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#include "GLFWWindow.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "Input/Input.h"
#include "Path/Path.h"
#include "RHI/Vulkan/VulkanContext.h"
#include "Utils/StringUtils.h"
#include "vulkan/vk_enum_string_helper.h"

PLATFORM_WINDOW_NAMESPACE_BEGIN

void GLFWWindowSurface::Initialize() {
    if (mAttachedInstanceHandle->IsValid()) {
        VkSurfaceKHR   Surface{};
        const VkResult Result = glfwCreateWindowSurface(mAttachedInstanceHandle->GetHandle(), mWindow, nullptr, &Surface);
        if (Result != VK_SUCCESS) {
            String ErrorStr = StringUtils::FromAnsiString(string_VkResult(Result));
            throw VulkanException(std::format(L"创建窗口表面失败: {}", ErrorStr));
        }
        mSurfaceHandle = vk::SurfaceKHR(Surface);
    }
}

vk::RenderPassCreateInfo ImGuiRenderPassProducer::GetRenderPassCreateInfo() {
    vk::AttachmentDescription ColorAttachment;
    ColorAttachment.format         = mSwapchainImageFormat;
    ColorAttachment.samples        = vk::SampleCountFlagBits::e1;
    ColorAttachment.loadOp         = vk::AttachmentLoadOp::eLoad;
    ColorAttachment.storeOp        = vk::AttachmentStoreOp::eStore;
    ColorAttachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
    ColorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    ColorAttachment.initialLayout  = vk::ImageLayout::ePresentSrcKHR;
    ColorAttachment.finalLayout    = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference ColorAttachmentRef;
    ColorAttachmentRef.attachment = 0;
    ColorAttachmentRef.layout     = vk::ImageLayout::eColorAttachmentOptimal;

    vk::SubpassDescription Subpass;
    Subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    Subpass.setColorAttachments(mColorAttachmentRef);

    vk::SubpassDependency Dependency;
    Dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
    Dependency.dstSubpass    = 0;
    Dependency.srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    Dependency.dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    Dependency.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    Dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

    mAttachments  = {ColorAttachment};
    mSubpasses    = {Subpass};
    mDependencies = {Dependency};

    vk::RenderPassCreateInfo RenderPassCreateInfo;
    RenderPassCreateInfo.setAttachments(mAttachments).setSubpasses(mSubpasses).setDependencies(mDependencies);
    return RenderPassCreateInfo;
}

ImGuiGraphicsPipeline::ImGuiGraphicsPipeline(const Ref<RHI::Vulkan::VulkanContext> InConext) : mContext(InConext) {
    Initialize();
    mContext.get().AddPipelineToRender(this);
}

void ImGuiGraphicsPipeline::Initialize() {
    if (mDescriptorPool != nullptr) return;
    ImGui_ImplVulkan_InitInfo Info{};
    Info.Instance       = mContext.get().GetVulkanInstance()->GetHandle();
    Info.PhysicalDevice = mContext.get().GetPhysicalDevice()->GetHandle();
    Info.Device         = mContext.get().GetLogicalDevice()->GetHandle();
    Info.QueueFamily    = mContext.get().GetPhysicalDevice()->FindQueueFamilyIndices().GraphicsFamily.value();
    Info.Queue          = mContext.get().GetLogicalDevice()->GetGraphicsQueue();
    Info.PipelineCache  = nullptr;
    Info.DescriptorPool = nullptr;   // 后面进行填充
    Info.Subpass        = 0;
    Info.MinImageCount  = mContext.get().GetSwapChainImageCount();
    Info.ImageCount     = mContext.get().GetSwapChainImageCount();
    Info.MSAASamples    = VK_SAMPLE_COUNT_1_BIT;

    CreateDescriptorPool();
    Info.DescriptorPool = mDescriptorPool;

    mCommandProducer =
        RHI::Vulkan::CommandProducer::CreateUnique(mContext.get().GetLogicalDevice(), vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    const auto RenderPassProducer = ImGuiRenderPassProducer::CreateUnique(mContext.get().GetSwapChainImageFormat());
    const auto RenderPassInfo     = RenderPassProducer->GetRenderPassCreateInfo();

    mRenderPass = RHI::Vulkan::RenderPass::CreateUnique(*mContext.get().GetLogicalDevice(), RenderPassInfo);

    // CommandBuffers
    vk::CommandBufferAllocateInfo AllocInfo = {};
    AllocInfo.setCommandPool(mContext.get().GetCommandProducer()->GetCommandPool())
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandBufferCount(mContext.get().GetSwapChainImageCount());

    mCommandBuffers = mContext.get().GetLogicalDevice()->GetHandle().allocateCommandBuffers(AllocInfo);

    // Framebuffers
    CreateFramebuffers();

    // 初始化Imgui
    ImGui_ImplVulkan_LoadFunctions(
        [](const char* Name, void* UserData) { return glfwGetInstanceProcAddress(static_cast<VkInstance>(UserData), Name); },
        mContext.get().GetVulkanInstance()->GetHandle()
    );
    ImGui_ImplVulkan_Init(&Info, mRenderPass->GetHandle());
    // 字体纹理
    CreateCommandBuffers();
}

void ImGuiGraphicsPipeline::Finialize() {
    if (mDescriptorPool == nullptr) return;
    CleanFramebuffers();
    // for (const auto& CommandBuffer: mCommandBuffers) {
    //     mContext.get().GetLogicalDevice()->GetHandle().freeCommandBuffers(
    //         mContext.get().GetCommandProducer()->GetCommandPool(), CommandBuffer
    //     );
    // }
    mCommandProducer->Finialize();
    mRenderPass->Finialize();
    ImGui_ImplVulkan_Shutdown();
    mContext.get().GetLogicalDevice()->GetHandle().destroyDescriptorPool(mDescriptorPool);
    mDescriptorPool = nullptr;
}

ImGuiGraphicsPipeline::~ImGuiGraphicsPipeline() {
    Finialize();
    mContext.get().RemovePipelineFromRender(this);
}

void ImGuiGraphicsPipeline::CreateFramebuffers() {
    using namespace RHI::Vulkan;
    auto& Context = static_cast<VulkanContext&>(mContext);
    mFramebuffers.resize(Context.GetSwapChainImageCount());
    for (size_t i = 0; i < mFramebuffers.size(); i++) {
        TArray<vk::ImageView> Attachments;
        Attachments = {
            Context.GetSwapChainImageViews()[i]->GetHandle(),
        };

        vk::FramebufferCreateInfo FramebufferInfo = {};
        FramebufferInfo   //
            .setRenderPass(mRenderPass->GetHandle())
            .setAttachments(Attachments)
            .setWidth(Context.GetSwapChainExtent().width)
            .setHeight(Context.GetSwapChainExtent().height)
            .setLayers(1);
        mFramebuffers[i] = Context.GetLogicalDevice()->GetHandle().createFramebuffer(FramebufferInfo);
    }
}

void ImGuiGraphicsPipeline::CleanFramebuffers() {
    for (const auto& Framebuffer: mFramebuffers) {
        mContext.get().GetLogicalDevice()->GetHandle().destroyFramebuffer(Framebuffer);
    }
}

void ImGuiGraphicsPipeline::CreateCommandBuffers() {
    vk::CommandBufferAllocateInfo AllocInfo = {};
    AllocInfo.level                         = vk::CommandBufferLevel::ePrimary;
    AllocInfo.commandPool                   = mCommandProducer->GetCommandPool();
    AllocInfo.commandBufferCount            = static_cast<uint32_t>(mFramebuffers.size());
    mCommandBuffers                         = mContext.get().GetLogicalDevice()->GetHandle().allocateCommandBuffers(AllocInfo);
}

void ImGuiGraphicsPipeline::SubmitGraphicsQueue(
    int CurrentImageIndex, vk::Queue InGraphicsQueue, TArray<vk::Semaphore> InWaitSemaphores, TArray<vk::Semaphore> InSingalSemaphores,
    vk::Fence InFrameFence
) {
    vk::CommandBufferBeginInfo BeginInfo = {};
    BeginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    mCommandBuffers[CurrentImageIndex].begin(&BeginInfo);
    vk::RenderPassBeginInfo         RenderPassInfo = {};
    TStaticArray<vk::ClearValue, 1> ClearValues    = {};
    RenderPassInfo.renderPass                      = mRenderPass->GetHandle();
    RenderPassInfo.framebuffer                     = mFramebuffers[CurrentImageIndex];
    RenderPassInfo.renderArea                      = vk::Rect2D{{0, 0}, mContext.get().GetSwapChainExtent()};
    RenderPassInfo.clearValueCount                 = static_cast<uint32_t>(ClearValues.size());
    RenderPassInfo.pClearValues                    = ClearValues.data();
    mCommandBuffers[CurrentImageIndex].beginRenderPass(RenderPassInfo, vk::SubpassContents::eInline);
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
    Info.waitSemaphoreCount = 0;
    InGraphicsQueue.submit(Info);
}

void ImGuiGraphicsPipeline::Rebuild() {
    CleanFramebuffers();
    CreateFramebuffers();
}

void ImGuiGraphicsPipeline::CreateDescriptorPool() {
    vk::DescriptorPoolSize       PoolSizes[] = {{vk::DescriptorType::eCombinedImageSampler, 1}};
    vk::DescriptorPoolCreateInfo PoolCreateInfo;
    PoolCreateInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet).setMaxSets(1).setPoolSizes(PoolSizes);
    mDescriptorPool = mContext.get().GetLogicalDevice()->GetHandle().createDescriptorPool(PoolCreateInfo);
}

TUniquePtr<GLFWWindowSurface> GlfwWindow::GetWindowSurface() {
    auto Surface = MakeUnique<GLFWWindowSurface>(nullptr, mWindowHandle);
    return Surface;
}

TArray<const char*> GlfwWindow::GetRequiredExtensions() const {
    TArray<const char*> Extensions;
    uint32_t            Count = 0;
    const char**        Names = glfwGetRequiredInstanceExtensions(&Count);
    for (uint32_t i = 0; i < Count; ++i) {
        Extensions.emplace_back(Names[i]);
    }
    return Extensions;
}

Size2D GlfwWindow::GetWindowSize() {
    glfwGetFramebufferSize(mWindowHandle, &mWidth, &mHeight);
    return Size2D(mWidth, mHeight);
}

void GlfwWindow::InitImGui(Ref<RHI::Vulkan::VulkanContext> InContext) {
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(mWindowHandle, true);
    mGraphicsPipeline             = MakeUnique<ImGuiGraphicsPipeline>(InContext);
    Path       DefaultFontPath    = L"Fonts/Maple_UI.ttf";
    AnsiString DefaultFontPathStr = DefaultFontPath.ToAnsiString();
    ImGuiIO&   IO                 = ImGui::GetIO();
    IO.Fonts->AddFontFromFileTTF(DefaultFontPathStr.c_str(), 30, nullptr, IO.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    ImGui_ImplVulkan_CreateFontsTexture();
}

void GlfwWindow::ShutdownImGui() {
    if (mGraphicsPipeline) mGraphicsPipeline->Finialize();
    ImGui_ImplGlfw_Shutdown();
}

void GlfwWindow::BeginImGuiFrame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GlfwWindow::EndImGuiFrame() {
    // ImGui::Render();
}

void GlfwWindow::Initialize() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    const AnsiString Title = StringUtils::ToAnsiString(mWindowTitle);
    mWindowHandle          = glfwCreateWindow(mWidth, mHeight, Title.c_str(), nullptr, nullptr);

    Camera = new Function::Camera(L"摄像机", nullptr);
    Camera->BeginPlay();
}

void GlfwWindow::Finalize() {
    mGraphicsPipeline->Finialize();
    glfwDestroyWindow(mWindowHandle);
    mWindowHandle = nullptr;
    glfwTerminate();
}

void GlfwWindow::Tick() {
    Camera->Tick();
    ImGui::Begin("Hello");
    ImGui::Text("Hello, world!");
    ImGui::Text("你好世界！");
    ImGui::End();
    ImGui::ShowDemoWindow();
    glfwPollEvents();
}

PLATFORM_WINDOW_NAMESPACE_END
