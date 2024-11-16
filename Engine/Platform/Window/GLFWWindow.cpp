/**
 * @file GLFWWindow.cpp
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#include "GLFWWindow.h"

#include "Core/CoreEvents.h"
#include "Platform/FileSystem/Path.h"
#include "IconsMaterialDesign.h"
#include "Platform/RHI/Vulkan/CommandBuffer.h"
#include "Platform/RHI/Vulkan/Render/CommandPool.h"
#include "Platform/RHI/Vulkan/Render/Framebuffer.h"
#include "Platform/RHI/Vulkan/Render/GraphicsPipeline.h"
#include "Platform/RHI/Vulkan/Render/RenderPass.h"
#include "Platform/RHI/Vulkan/VulkanContext.h"
#include "vulkan/vulkan_to_string.hpp"
#include "WindowCommon.h"

#include "Core/Profiler/ProfileMacro.h"

#ifdef USE_IMGUI
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#endif

using namespace rhi::vulkan;
using namespace core;

namespace platform::window
{

#ifdef USE_IMGUI
class ImGuiRenderPass : public RenderPass
{
public:
    explicit ImGuiRenderPass(const core::StringView& debug_name) :
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
            info.renderPass   = handle_;
            Array attachments = {VulkanContext::Get()->GetSwapChainImageViews()[i]->GetHandle()};
            info.setAttachments(attachments);
            info.width            = width_;
            info.height           = height_;
            info.layers           = 1;
            framebuffer_names_[i] = std::string("ImGuiFrameBuffer_") + std::to_string(i);
            framebuffers_[i]      = New<Framebuffer>(info, framebuffer_names_[i].Data());
        }
    }

    void CleanFrameBuffer() override
    {
        for (auto& framebuffer: framebuffers_)
        {
            Delete(framebuffer);
        }
        framebuffers_.clear();
    }

    void ResizeFramebuffer(int w, int h) override
    {
        if (w > 0 && h > 0)
        {
            width_  = w;
            height_ = h;
            CleanFrameBuffer();
            SetupFramebuffer();
        }
    }

    [[nodiscard]] Framebuffer* GetFramebuffer(int index) const { return framebuffers_[index]; }

    vk::Framebuffer GetCurrentFramebufferHandle() override
    {
        NEVER_ENTRY_WARN(logcat::Platform_Window);
        return nullptr;
    }

    Array<Framebuffer*> framebuffers_;
    Array<String>       framebuffer_names_;
};

class RealImGuiGraphicsPipeline : public ImguiGraphicsPipeline
{
public:
    explicit RealImGuiGraphicsPipeline()
    {
        context_ = VulkanContext::Get();
        Initialize();
        OnAppWindowResized.AddBind(this, &RealImGuiGraphicsPipeline::Rebuild);
    }

    void Initialize()
    {
        if (descriptor_pool_ != nullptr) return;
        ImGui_ImplVulkan_InitInfo Info{};
        Info.Instance       = context_->GetVulkanInstance()->GetHandle();
        Info.PhysicalDevice = context_->GetPhysicalDevice()->GetHandle();
        Info.Device         = context_->GetLogicalDevice()->GetHandle();
        Info.QueueFamily    = context_->GetPhysicalDevice()->FindQueueFamilyIndices().graphics_family.value();
        Info.Queue          = context_->GetLogicalDevice()->GetGraphicsQueue();
        // TODO: 填充PipelineCache
        Info.PipelineCache  = nullptr;
        Info.Subpass        = 0;
        Info.MinImageCount  = context_->GetSwapChainImageCount();
        Info.ImageCount     = context_->GetSwapChainImageCount();
        Info.MSAASamples    = VK_SAMPLE_COUNT_1_BIT;

        CreateDescriptorPool();
        Info.DescriptorPool = descriptor_pool_;

        command_pool_ =
            CommandPool::CreateUnique(context_->GetLogicalDevice().Get(), vk::CommandPoolCreateFlagBits::eResetCommandBuffer, "ImGuiCommandPool");

        render_pass_ = New<ImGuiRenderPass>("ImGuiRenderPass");
        render_pass_->Initialize();

        // 初始化Imgui
        ImGui_ImplVulkan_LoadFunctions(
            [](const char* Name, void* UserData) { return glfwGetInstanceProcAddress(static_cast<VkInstance>(UserData), Name); },
            context_->GetVulkanInstance()->GetHandle()
        );
        ImGui_ImplVulkan_Init(&Info, render_pass_->GetHandle());
    }

    void Finialize()
    {
        if (descriptor_pool_ == nullptr) return;
        command_pool_->DeInitialize();
        render_pass_->CleanFrameBuffer();
        Delete(render_pass_);
        render_pass_ = nullptr;
        ImGui_ImplVulkan_Shutdown();
        context_->GetLogicalDevice()->DestroyDescriptorPool(descriptor_pool_);
        descriptor_pool_ = nullptr;
    }

    ~RealImGuiGraphicsPipeline() override { Finialize(); }

protected:
    void CreateDescriptorPool()
    {
        vk::DescriptorPoolSize       PoolSizes[] = {{vk::DescriptorType::eCombinedImageSampler, 1}};
        vk::DescriptorPoolCreateInfo PoolCreateInfo;
        PoolCreateInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet).setMaxSets(50).setPoolSizes(PoolSizes);
        descriptor_pool_ = context_->GetLogicalDevice()->GetHandle().createDescriptorPool(PoolCreateInfo);
    }

public:
    void Draw(vk::CommandBuffer cb) override
    {
        PROFILE_SCOPE_AUTO;
        vk::CommandBufferBeginInfo begin_info          = {};
        int32_t                    current_image_index = g_engine_statistics.current_image_index;
        begin_info.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        vk::RenderPassBeginInfo render_pass_info = {};
        render_pass_info.renderPass              = render_pass_->GetHandle();
        render_pass_info.framebuffer             = render_pass_->GetFramebuffer(current_image_index)->GetHandle();
        render_pass_info.renderArea              = vk::Rect2D{{0, 0}, context_->GetSwapChainExtent()};
        rhi::GetGfxContext().BeginProfile("ImGui Pass GPU", CommandBufferVulkan(cb));
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
        auto* draw_data = ImGui::GetDrawData();
        ImGui_ImplVulkan_RenderDrawData(draw_data, cb);
        cb.endRenderPass();
        rhi::GetGfxContext().EndProfile();
    }

    void Rebuild(int w, int h) override
    {
        if (w == 0 || h == 0)
        {
            return;
        }
        render_pass_->ResizeFramebuffer(w, h);
        // 重新根据窗口大小调整ImGui Window的大小
    }

private:
    VulkanContext* context_;

    vk::DescriptorPool     descriptor_pool_ = nullptr;
    UniquePtr<CommandPool> command_pool_;
    ImGuiRenderPass*       render_pass_ = nullptr;
};

#endif

void GLFWWindowSurface::Initialize()
{
    if (mAttachedInstanceHandle->IsValid())
    {
        VkSurfaceKHR   surface{};
        const VkResult result = glfwCreateWindowSurface(mAttachedInstanceHandle->GetHandle(), mWindow, nullptr, &surface);
        Assert(logcat::Platform_Window, result == VK_SUCCESS, "Failed to create window surface.");
        mSurfaceHandle = vk::SurfaceKHR(surface);
    }
}

UniquePtr<GLFWWindowSurface> GlfwWindow::GetWindowSurface()
{
    auto Surface = MakeUnique<GLFWWindowSurface>(nullptr, window_handle_);
    return Surface;
}

Array<const char*> GlfwWindow::GetRequiredExtensions() const
{
    Array<const char*> Extensions;
    uint32_t           Count = 0;
    const char**       Names = glfwGetRequiredInstanceExtensions(&Count);
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

#ifdef USE_IMGUI
void GlfwWindow::InitImGui(VulkanContext* InContext)
{
    ImGui::CreateContext();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui_ImplGlfw_InitForVulkan(window_handle_, true);
    imgui_graphics_pipeline_ = New<RealImGuiGraphicsPipeline>();
    // function::OnRequireImGuiGraphicsPipeline.Bind(this, &ThisClass::RegisterImGuiPipeline);
    SetupImGuiFonts();
}

void GlfwWindow::SetupImGuiFonts()
{
    ImGuiIO&                 io = ImGui::GetIO();
    File                     default_font_path{EDITOR_UI_FONT_PATH};
    String                   default_font_path_str = default_font_path.GetAbsolutePath();
    ImVector<ImWchar>        ranges;
    ImFontGlyphRangesBuilder builder;
    builder.AddRanges(io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    builder.AddText(IMGUI_FONT_STR);
    builder.BuildRanges(&ranges);
    io.Fonts->AddFontFromFileTTF(default_font_path_str.Data(), WINDOW_SCALE(DEFAULT_FONT_SIZE), nullptr, ranges.Data);

    // 字体图标
    static constexpr ImWchar icon_font_ranges[] = {ICON_MIN_MD, ICON_MAX_16_MD, 0};
    ImFontConfig             icon_font_config;
    icon_font_config.MergeMode        = true;
    icon_font_config.GlyphOffset      = {WINDOW_SCALE(0.f), WINDOW_SCALE(4.f)};
    icon_font_config.GlyphMinAdvanceX = WINDOW_SCALE(20.f);

    File   icon_font{GOOGLE_MATERIAL_ICON_FONT_PATH};
    String icon_font_ansi = icon_font.GetAbsolutePath();
    io.Fonts->AddFontFromFileTTF(icon_font_ansi.Data(), WINDOW_SCALE(DEFAULT_FONT_SIZE), &icon_font_config, icon_font_ranges);

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
    ImGui::DockSpaceOverViewport();
}

void GlfwWindow::EndImGuiFrame()
{
    // ImGui::Render();
}

void GlfwWindow::RegisterImGuiPipeline(rhi::vulkan::ImguiGraphicsPipeline** pipeline) const
{
    *pipeline = imgui_graphics_pipeline_;
}
#endif

void GlfwWindow::Initialize()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window_handle_ = glfwCreateWindow(width_, height_, window_title_.Data(), nullptr, nullptr);
}

void GlfwWindow::Finalize()
{
    if (imgui_graphics_pipeline_)
    {
        Delete(imgui_graphics_pipeline_);
        imgui_graphics_pipeline_ = nullptr;
    }
    glfwDestroyWindow(window_handle_);
    window_handle_ = nullptr;
    glfwTerminate();
}

void GlfwWindow::Tick(float DeltaTime)
{
    glfwPollEvents();
}

void GlfwWindow::SetMouseVisible(const bool InVisible) const
{
    glfwSetInputMode(window_handle_, GLFW_CURSOR, InVisible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

}
