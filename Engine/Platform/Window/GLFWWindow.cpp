/**
 * @file GLFWWindow.cpp
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#include "GLFWWindow.h"
#include "GameObject/GameObject.h"
#include "IconsMaterialDesign.h"
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
#include "vulkan/vulkan_to_string.hpp"

using namespace RHI::Vulkan;

PLATFORM_WINDOW_NAMESPACE_BEGIN

class ImGuiRenderPass : public RenderPass
{
public:
    explicit ImGuiRenderPass(const AnsiString& debug_name) :
        RenderPass(g_engine_statistics.window_size.width, g_engine_statistics.window_size.height, debug_name)
    {
    }

protected:
    void SetupAttachments() override
    {
        RenderPassAttachmentParam param{};
        param.format           = VulkanContext::Get()->GetSwapChainImageFormat();
        param.sample_count     = vk::SampleCountFlagBits::e1;
        param.load_op          = vk::AttachmentLoadOp::eDontCare;
        param.store_op         = vk::AttachmentStoreOp::eStore;
        param.stencil_load_op  = vk::AttachmentLoadOp::eDontCare;
        param.stencil_store_op = vk::AttachmentStoreOp::eDontCare;
        param.initial_layout   = vk::ImageLayout::eUndefined;
        param.final_layout     = vk::ImageLayout::ePresentSrcKHR;
        param.reference_layout = vk::ImageLayout::eColorAttachmentOptimal;
        NewSwapchainColorAttachment(param);
    }

    void SetupSubpassDependency() override
    {
        vk::SubpassDependency dependency;
        dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass    = 0;
        dependency.srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dependency.dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dependency.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
        dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
        dependencies_.push_back(dependency);
    }

public:
    void SetupFramebuffer() override
    {
        framebuffers_.resize(g_engine_statistics.graphics.swapchain_image_count);
        framebuffer_names_.resize(g_engine_statistics.graphics.swapchain_image_count);
        for (int i = 0; i < g_engine_statistics.graphics.swapchain_image_count; ++i)
        {
            vk::FramebufferCreateInfo info{};
            info.renderPass    = handle_;
            TArray attachments = {VulkanContext::Get()->GetSwapChainImageViews()[i]->GetHandle()};
            info.setAttachments(attachments);
            info.width            = width_;
            info.height           = height_;
            info.layers           = 1;
            framebuffer_names_[i] = std::string("ImGuiFrameBuffer_") + std::to_string(i);
            framebuffers_[i]      = new Framebuffer(info, framebuffer_names_[i].c_str());
        }
    }

    void CleanFrameBuffer() override
    {
        for (auto& framebuffer: framebuffers_)
        {
            delete framebuffer;
        }
        framebuffers_.clear();
    }

    Framebuffer* GetFramebuffer(int index) const { return framebuffers_[index]; }

    vk::Framebuffer GetCurrentFramebufferHandle() override;

    TArray<Framebuffer*> framebuffers_;
    TArray<AnsiString>   framebuffer_names_;
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

public:
    void Draw(vk::CommandBuffer cb) override;

    void Rebuild(int w, int h) override
    {
        render_pass_->ResizeFramebuffer(w, h);
    }

private:
    VulkanContext* context_;

    vk::DescriptorPool      descriptor_pool_ = nullptr;
    TUniquePtr<CommandPool> command_pool_;
    ImGuiRenderPass*        render_pass_ = nullptr;
};

void GLFWWindowSurface::Initialize()
{
    if (mAttachedInstanceHandle->IsValid())
    {
        VkSurfaceKHR   surface{};
        const VkResult result = glfwCreateWindowSurface(mAttachedInstanceHandle->GetHandle(), mWindow, nullptr, &surface);
        if (result != VK_SUCCESS)
        {
            String error_str = StringUtils::FromAnsiString(to_string(static_cast<vk::Result>(result)));
            throw VulkanException(std::format(L"创建窗口表面失败: {}", error_str));
        }
        mSurfaceHandle = vk::SurfaceKHR(surface);
    }
}

vk::Framebuffer ImGuiRenderPass::GetCurrentFramebufferHandle()
{
    NEVER_ENTRY_WARNING()
    return nullptr;
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

    command_pool_ = CommandPool::CreateUnique(context_->GetLogicalDevice(), vk::CommandPoolCreateFlagBits::eResetCommandBuffer, "ImGuiCommandPool");

    render_pass_ = new ImGuiRenderPass("ImGuiRenderPass");
    render_pass_->Initialize();

    // 初始化Imgui
    ImGui_ImplVulkan_LoadFunctions(
        [](const char* Name, void* UserData) { return glfwGetInstanceProcAddress(static_cast<VkInstance>(UserData), Name); },
        context_->GetVulkanInstance()->GetHandle()
    );
    ImGui_ImplVulkan_Init(&Info, render_pass_->GetHandle());
}

void ImGuiGraphicsPipeline::Finialize()
{
    if (descriptor_pool_ == nullptr) return;
    command_pool_->Finialize();
    render_pass_->CleanFrameBuffer();
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

void ImGuiGraphicsPipeline::Draw(vk::CommandBuffer cb)
{
    vk::CommandBufferBeginInfo begin_info          = {};
    int32_t                    current_image_index = g_engine_statistics.current_image_index;
    begin_info.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    vk::RenderPassBeginInfo render_pass_info = {};
    render_pass_info.renderPass              = render_pass_->GetHandle();
    render_pass_info.framebuffer             = render_pass_->GetFramebuffer(current_image_index)->GetHandle();
    render_pass_info.renderArea              = vk::Rect2D{{0, 0}, context_->GetSwapChainExtent()};
    cb.beginRenderPass(render_pass_info, vk::SubpassContents::eInline);

    ImGui::Render();
    vk::Viewport viewport;
    viewport.x        = 0;
    viewport.y        = 0;
    viewport.width    = g_engine_statistics.window_size.width;
    viewport.height   = g_engine_statistics.window_size.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    cb.setViewport(0, viewport);
    vk::Rect2D scisor;
    scisor.offset = vk::Offset2D{0, 0};
    scisor.extent = VulkanContext::Get()->GetSwapChainExtent();
    cb.setScissor(0, scisor);

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cb);
    cb.endRenderPass();
}

void ImGuiGraphicsPipeline::CreateDescriptorPool()
{
    vk::DescriptorPoolSize       PoolSizes[] = {{vk::DescriptorType::eCombinedImageSampler, 1}};
    vk::DescriptorPoolCreateInfo PoolCreateInfo;
    PoolCreateInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet).setMaxSets(10).setPoolSizes(PoolSizes);
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
    ImGuiIO&                 io                    = ImGui::GetIO();
    Path                     default_font_path     = MAPLE_UI_FONT_PATH;
    AnsiString               default_font_path_str = default_font_path.ToAbsoluteAnsiString();
    ImVector<ImWchar>        ranges;
    ImFontGlyphRangesBuilder builder;
    builder.AddRanges(io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    builder.AddText(IMGUI_FONT_STR);
    builder.BuildRanges(&ranges);
    io.Fonts->AddFontFromFileTTF(default_font_path_str.c_str(), DEFAULT_FONT_SIZE, nullptr, ranges.Data);

    // 字体图标
    static constexpr ImWchar icon_font_ranges[] = {ICON_MIN_MD, ICON_MAX_16_MD, 0};
    ImFontConfig             icon_font_config;
    icon_font_config.MergeMode   = true;
    icon_font_config.GlyphOffset = {0.f, 4.f};

    Path       icon_font      = GOOGLE_MATERIAL_ICON_FONT_PATH;
    AnsiString icon_font_ansi = icon_font.ToAbsoluteAnsiString();
    io.Fonts->AddFontFromFileTTF(icon_font_ansi.c_str(), DEFAULT_FONT_SIZE, &icon_font_config, icon_font_ranges);

    ImGui_ImplVulkan_CreateFontsTexture();
}

void GlfwWindow::ShutdownImGui() const
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
    glfwPollEvents();
    Input::InternalTick();
}

void GlfwWindow::SetMouseVisible(const bool InVisible) const
{
    glfwSetInputMode(window_handle_, GLFW_CURSOR, InVisible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

void GlfwWindow::RegisterImGuiPipeline(RHI::Vulkan::ImguiGraphicsPipeline** pipeline) const
{
    *pipeline = imgui_graphics_pipeline_;
}

PLATFORM_WINDOW_NAMESPACE_END
