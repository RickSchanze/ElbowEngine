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
#include "Render/Event.h"
#include "RHI/Vulkan/Render/CommandPool.h"
#include "RHI/Vulkan/Render/Framebuffer.h"
#include "RHI/Vulkan/Render/GraphicsPipeline.h"
#include "RHI/Vulkan/Render/RenderPass.h"
#include "RHI/Vulkan/VulkanContext.h"
#include "Utils/StringUtils.h"
#include "vulkan/vk_enum_string_helper.h"

using namespace RHI::Vulkan;

PLATFORM_WINDOW_NAMESPACE_BEGIN

class ImGuiRenderPass : public RenderPass
{
protected:
    void OnCreateAttachments() override
    {
        RenderPassAttachmentParam Param(vk::ImageUsageFlagBits::eSampled);
        Param.InitialLayout   = vk::ImageLayout::ePresentSrcKHR;
        Param.finial_layout    = vk::ImageLayout::ePresentSrcKHR;
        Param.reference_layout = vk::ImageLayout::eColorAttachmentOptimal;
        Param.StoreOp         = vk::AttachmentStoreOp::eStore;
        Param.LoadOp          = vk::AttachmentLoadOp::eDontCare;
        NewAttachment(Param, true);
    }

    void CreateSubpassDescription() override;
};

class ImGuiGraphicsPipeline : public ImguiGraphicsPipeline
{
public:
    explicit ImGuiGraphicsPipeline();

    void Initialize();
    void Finialize();

    ~ImGuiGraphicsPipeline() override;

protected:
    void CreateDescriptorPool();
    void CreateCommandBuffers();

public:
    void Draw() override;

    void Rebuild();

    vk::CommandBuffer GetCurrentCommandBuffer() const override;

private:
    VulkanContext* context_;

    vk::DescriptorPool        descriptor_pool_ = nullptr;
    TUniquePtr<CommandPool>   command_pool_;
    RenderPass*               render_pass_ = nullptr;
    TArray<vk::CommandBuffer> command_buffers_;
};

void GLFWWindowSurface::Initialize()
{
    if (mAttachedInstanceHandle->IsValid())
    {
        VkSurfaceKHR   Surface{};
        const VkResult Result = glfwCreateWindowSurface(mAttachedInstanceHandle->GetHandle(), mWindow, nullptr, &Surface);
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
    dependency_.srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency_.dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency_.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    dependency_.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
}

ImGuiGraphicsPipeline::ImGuiGraphicsPipeline()
{
    context_ = VulkanContext::Get();
    Initialize();
}

void ImGuiGraphicsPipeline::Initialize()
{
    if (descriptor_pool_ != nullptr) return;
    ImGui_ImplVulkan_InitInfo Info{};
    Info.Instance       = context_->GetVulkanInstance()->GetHandle();
    Info.PhysicalDevice = context_->GetPhysicalDevice()->GetHandle();
    Info.Device         = context_->GetLogicalDevice()->GetHandle();
    Info.QueueFamily    = context_->GetPhysicalDevice()->FindQueueFamilyIndices().graphics_family.value();
    Info.Queue          = context_->GetLogicalDevice()->GetGraphicsQueue();
    // TODO: 填充PipelineCache和DescriptorPool
    Info.PipelineCache  = nullptr;
    Info.DescriptorPool = nullptr;
    Info.Subpass        = 0;
    Info.MinImageCount  = context_->GetSwapChainImageCount();
    Info.ImageCount     = context_->GetSwapChainImageCount();
    Info.MSAASamples    = VK_SAMPLE_COUNT_1_BIT;

    CreateDescriptorPool();
    Info.DescriptorPool = descriptor_pool_;

    command_pool_ = CommandPool::CreateUnique(context_->GetLogicalDevice(), vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

    render_pass_ = new ImGuiRenderPass();
    render_pass_->Initialize();

    // CommandBuffers
    vk::CommandBufferAllocateInfo AllocInfo = {};
    AllocInfo.setCommandPool(context_->GetCommandPool()->GetHandle())
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandBufferCount(context_->GetSwapChainImageCount());

    command_buffers_ = context_->GetLogicalDevice()->AllocateCommandBuffers(AllocInfo);

    // 初始化Imgui
    ImGui_ImplVulkan_LoadFunctions(
        [](const char* Name, void* UserData) { return glfwGetInstanceProcAddress(static_cast<VkInstance>(UserData), Name); },
        context_->GetVulkanInstance()->GetHandle()
    );
    ImGui_ImplVulkan_Init(&Info, render_pass_->GetHandle());
    // 字体纹理
    CreateCommandBuffers();
}

void ImGuiGraphicsPipeline::Finialize()
{
    if (descriptor_pool_ == nullptr) return;
    command_pool_->Finialize();
    delete render_pass_;
    render_pass_ = nullptr;
    ImGui_ImplVulkan_Shutdown();
    context_->GetLogicalDevice()->DestroyDescriptorPool(descriptor_pool_);
    descriptor_pool_ = nullptr;
}

ImGuiGraphicsPipeline::~ImGuiGraphicsPipeline()
{
    Finialize();
}

void ImGuiGraphicsPipeline::CreateCommandBuffers()
{
    vk::CommandBufferAllocateInfo AllocInfo = {};
    AllocInfo.level                         = vk::CommandBufferLevel::ePrimary;
    AllocInfo.commandPool                   = command_pool_->GetHandle();
    AllocInfo.commandBufferCount            = static_cast<uint32_t>(render_pass_->GetFrameBuffers().size());
    command_buffers_                        = context_->GetLogicalDevice()->AllocateCommandBuffers(AllocInfo);
}

void ImGuiGraphicsPipeline::Draw()
{
    vk::CommandBufferBeginInfo begin_info          = {};
    int32_t                    current_image_index = g_engine_statistics.current_image_index;
    begin_info.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    command_buffers_[current_image_index].begin(&begin_info);
    vk::RenderPassBeginInfo         render_pass_info = {};
    TStaticArray<vk::ClearValue, 1> clear_values     = {};
    render_pass_info.renderPass                      = render_pass_->GetHandle();
    render_pass_info.framebuffer                     = render_pass_->GetFrameBuffer(current_image_index)->GetHandle();
    render_pass_info.renderArea                      = vk::Rect2D{{0, 0}, context_->GetSwapChainExtent()};
    render_pass_info.clearValueCount                 = static_cast<uint32_t>(clear_values.size());
    render_pass_info.pClearValues                    = clear_values.data();
    command_buffers_[current_image_index].beginRenderPass(render_pass_info, vk::SubpassContents::eInline);
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffers_[current_image_index]);
    command_buffers_[current_image_index].endRenderPass();
    command_buffers_[current_image_index].end();
    // vk::PipelineStageFlags WaitFlag = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    // vk::SubmitInfo         Info     = {};
    // Info.setSignalSemaphores(InSingalSemaphores)
    //     .setWaitSemaphores(InWaitSemaphores)
    //     .setCommandBuffers(command_buffers_[current_image_index])
    //     .setWaitDstStageMask(WaitFlag);
    // InGraphicsQueue.submit(Info);
}

void ImGuiGraphicsPipeline::Rebuild()
{
    render_pass_->Rebuild(false);
}

vk::CommandBuffer ImGuiGraphicsPipeline::GetCurrentCommandBuffer() const
{
    return command_buffers_[g_engine_statistics.current_image_index];
}

void ImGuiGraphicsPipeline::CreateDescriptorPool()
{
    vk::DescriptorPoolSize       PoolSizes[] = {{vk::DescriptorType::eCombinedImageSampler, 1}};
    vk::DescriptorPoolCreateInfo PoolCreateInfo;
    PoolCreateInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet).setMaxSets(1).setPoolSizes(PoolSizes);
    descriptor_pool_ = context_->GetLogicalDevice()->GetHandle().createDescriptorPool(PoolCreateInfo);
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

void GlfwWindow::InitImGui(Ref<VulkanContext> InContext)
{
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(window_handle_, true);
    imgui_graphics_pipeline_ = new ImGuiGraphicsPipeline();
    Function::OnRequireImGuiGraphicsPipeline.AddObject(this, &ThisClass::RegisterImGuiPipeline);
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
    window_handle_         = glfwCreateWindow(width_, height_, Title.c_str(), nullptr, nullptr);

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

void GlfwWindow::RegisterImGuiPipeline(RHI::Vulkan::ImguiGraphicsPipeline** pipeline){
    *pipeline = imgui_graphics_pipeline_;
}

PLATFORM_WINDOW_NAMESPACE_END
