/**
 * @file GLFWWindow.h
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "WindowCommon.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "imgui_impl_vulkan.h"
#include "Math/MathTypes.h"
#include "RHI/Vulkan/Instance.h"
#include "RHI/Vulkan/Interface/IGraphicsPipeline.h"
#include "RHI/Vulkan/Interface/IRenderPassProducer.h"
namespace RHI::Vulkan {
class RenderPass;
}
namespace RHI::Vulkan {
class VulkanContext;
}
PLATFORM_WINDOW_NAMESPACE_BEGIN

struct GLFWWindowSurface : RHI::Vulkan::SurfaceBase
{
    explicit GLFWWindowSurface(RHI::Vulkan::Instance* InParentInstance, GLFWwindow* InWindow) :
        SurfaceBase(InParentInstance), mWindow(InWindow) {}

    void Initialize() override;

    GLFWwindow* mWindow;
};

class ImGuiRenderPassProducer final : public RHI::Vulkan::IRenderPassProducer {
protected:
    struct Protected
    {};

public:
    static TUniquePtr<IRenderPassProducer> CreateUnique(vk::Format InSwapchainImageFormat) {
        return MakeUnique<ImGuiRenderPassProducer>(Protected{}, InSwapchainImageFormat);
    }

    ImGuiRenderPassProducer(Protected, const vk::Format InSwapChainImageFormat) : mSwapchainImageFormat(InSwapChainImageFormat) {}

    vk::RenderPassCreateInfo GetRenderPassCreateInfo() override;

private:
    // 与引擎渲染使用同一套Swapchain、SwapChainImageFormat以及SampleCount
    vk::Format mSwapchainImageFormat{};

    // 不需要深度
    vk::AttachmentReference mColorAttachmentRef{0, vk::ImageLayout::eColorAttachmentOptimal};

    TArray<vk::AttachmentDescription> mAttachments;
    TArray<vk::SubpassDescription>    mSubpasses;
    TArray<vk::SubpassDependency>     mDependencies;
};

class ImGuiGraphicsPipeline : public IGraphicsPipeline {
public:

    explicit ImGuiGraphicsPipeline(Ref<RHI::Vulkan::VulkanContext> InConext);

    void Initialize();
    void Finialize();

    ~ImGuiGraphicsPipeline() override;

protected:
    void CreateDescriptorPool();
    void CreateFramebuffers();
    void CleanFramebuffers();
    void CreateCommandBuffers();

public:
    void SubmitGraphicsQueue(
        int CurrentImageIndex, vk::Queue InGraphicsQueue, TArray<vk::Semaphore> InWaitSemaphores, TArray<vk::Semaphore> InSingalSemaphores,
        vk::Fence InFrameFence
    ) override;

    void Rebuild() override;

private:
    Ref<RHI::Vulkan::VulkanContext> mContext;

    vk::DescriptorPool                      mDescriptorPool = nullptr;
    TUniquePtr<RHI::Vulkan::CommandProducer> mCommandProducer;
    TUniquePtr<RHI::Vulkan::RenderPass>      mRenderPass;
    TArray<vk::CommandBuffer>                mCommandBuffers;
    TArray<vk::Framebuffer>                  mFramebuffers;
};

class GlfwWindow {
public:
    explicit GlfwWindow(String InWindowTitle, const int InWidth, const int InHeight) :
        mWindowTitle(Move(InWindowTitle)), mWidth(InWidth), mHeight(InHeight) {}

    [[nodiscard]] bool IsValid() const { return mWindowHandle != nullptr; }

    TUniquePtr<GLFWWindowSurface>     GetWindowSurface();
    [[nodiscard]] TArray<const char*> GetRequiredExtensions() const;
    [[nodiscard]] GLFWwindow*        GetGLFWWindowHandle() const { return mWindowHandle; }

    Size2D GetWindowSize();

    void SetFrameBufferResizedCallback(const GLFWframebuffersizefun Callback) const {
        glfwSetFramebufferSizeCallback(mWindowHandle, Callback);
    }

    void InitImGui(Ref<RHI::Vulkan::VulkanContext> InContext);
    void ShutdownImGui();

    void BeginImGuiFrame();
    void EndImGuiFrame();

    void Initialize();
    void Finalize();
    void Tick();

private:
    GLFWwindow* mWindowHandle = nullptr;
    String      mWindowTitle;

    TUniquePtr<ImGuiGraphicsPipeline> mGraphicsPipeline;

    int mWidth;
    int mHeight;
};

PLATFORM_WINDOW_NAMESPACE_END
