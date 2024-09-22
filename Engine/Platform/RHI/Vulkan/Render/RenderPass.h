/**
 * @file RenderPass.h
 * @author Echo 
 * @Date 24-4-25
 * @brief 
 */

#pragma once
#include "RHI/Vulkan/Interface/IRHIResource.h"
#include "Utils/StringUtils.h"
#include "vulkan/vulkan.hpp"

namespace RHI::Vulkan
{
class RenderPass;
}

#define REGISTER_RENDER_PASS_REFL(full_qualified_class_name)                               \
    RTTR_REGISTRATION                                                                      \
    {                                                                                      \
        rttr::registration::class_<full_qualified_class_name>(#full_qualified_class_name); \
    }

RHI_VULKAN_NAMESPACE_BEGIN

class LogicalDevice;
class Framebuffer;
class ImageView;
class Image;

struct RenderPassAttachmentParam
{
    vk::Format              format           = vk::Format::eUndefined;   // Undefined则会使用交换链图像格式
    vk::SampleCountFlagBits sample_count     = vk::SampleCountFlagBits::e1;
    vk::AttachmentLoadOp    load_op          = vk::AttachmentLoadOp::eClear;
    vk::AttachmentStoreOp   store_op         = vk::AttachmentStoreOp::eStore;
    vk::AttachmentLoadOp    stencil_load_op  = vk::AttachmentLoadOp::eDontCare;
    vk::AttachmentStoreOp   stencil_store_op = vk::AttachmentStoreOp::eDontCare;
    vk::ImageLayout         initial_layout   = vk::ImageLayout::eUndefined;
    // 自动决定，如果SampleCount不为e1则是ColorAttachmentOptimal 否则 ePresentSrcKHR
    vk::ImageLayout         final_layout     = vk::ImageLayout::eUndefined;
    vk::ImageLayout         reference_layout = vk::ImageLayout::eUndefined;

    void Init();
};

struct RenderPassAttachmentImageInfo
{
    vk::ImageUsageFlags usage;
    vk::Format          format;
    vk::ImageLayout     InitialLayout;
    vk::ImageLayout     FinalLayout;

    const char* debug_image_name;
    const char* debug_image_view_name;
};

/**
 * 基本RenderPass 基本RenderPass包含一个ColorAttachment 一个DepthAttachment和一个Multismaple使用的Attachment
 * RenderPass同时还包含了对应的Framebuffer
 */
class RenderPass : public IRHIResource
{
public:
    typedef RenderPass ThisClass;

    struct RenderTarget
    {
    public:
        Image*       image       = nullptr;
        ImageView*   image_view  = nullptr;
        Framebuffer* framebuffer = nullptr;

        bool is_swapchain = false;

        ~RenderTarget();
    };

    bool IsValid() const;

    explicit RenderPass(uint32_t width, uint32_t height, const AnsiString& debug_name = "");

    ~RenderPass() override;

    void Initialize();

    virtual void PostInitialize() {}

    // 初始化Framebuffer和其Attachment
    // 用于创建RenderPass需要的所有Attachment
    virtual void            SetupAttachments()            = 0;
    virtual void            SetupFramebuffer()            = 0;
    virtual void            CleanFrameBuffer()            = 0;
    virtual vk::Framebuffer GetCurrentFramebufferHandle() = 0;
    virtual void            SetupSubpassDependency()      = 0;

    virtual void ResizeFramebuffer(int w, int h);

    virtual void SetupSubpassDescription();
    virtual void Begin(vk::CommandBuffer cb, const Color& clear_color);
    virtual void End(vk::CommandBuffer cb);
    void         Destroy() override;

protected:
    void InternalDestroy();

public:
    vk::RenderPass GetHandle() const { return handle_; }

    // Config
    vk::SampleCountFlagBits sample_count = vk::SampleCountFlagBits::e1;

protected:
    /**
     * 为这个RenderPass新加一个Attachment
     * @param param Attachment参数
     * @param attach_to_swapchain 这个Attachment是不是要附着到交换链Index
     * @param is_depth 这个Attachment是不是深度Attachment
     * @param is_sample_resolve 这个Attachment是不是用来转换多重采样
     */
    void NewAttachment(RenderPassAttachmentParam& param, bool attach_to_swapchain = false, bool is_depth = false, bool is_sample_resolve = false);

    void NewDepthAttachment(RenderPassAttachmentParam& param);
    void NewSampleResolveAttachment(RenderPassAttachmentParam& param);
    void NewSwapchainColorAttachment(RenderPassAttachmentParam& param);

    void CreateRenderPass();

    vk::RenderPass handle_ = VK_NULL_HANDLE;

    TArray<vk::AttachmentDescription> attachment_descs_;
    TArray<vk::AttachmentReference>   attahcment_refs_;
    TArray<vk::AttachmentReference>   subpass_color_attachment_refs_;

    // 深度附着一个RenderPass最多只允许一个
    int32_t depth_attachment_index_ = -1;
    int32_t sample_resolve_index_   = -1;
    // 附着到交换链的图像Index
    int32_t swapchain_view_index_   = -1;

    TArray<vk::SubpassDependency> dependencies_;

    vk::SubpassDescription subpass_;

    uint32_t width_;
    uint32_t height_;

    AnsiString render_pass_name_;
};

class RenderPassManager final : public Singleton<RenderPassManager>
{
public:
    RenderPassManager();

    template<typename T>
    static T* GetRenderPass();

    template<typename T>
    static T* CreateRenderPass(uint32_t width = 0, uint32_t height = 0, const AnsiString& name = "");

    template<typename T>
    static T* GetOrCreateRenderPass(uint32_t width = 0, uint32_t height = 0, const AnsiString& name = "");

    RenderPass* GetRenderPass(const Type &t) { return render_passes_.contains(t) ? render_passes_[t] : nullptr; }

    static void DestroyRenderPasses();

private:
    THashMap<Type, RenderPass*> render_passes_;
};

template<typename T>
T* RenderPassManager::GetRenderPass()
{
    static_assert(std::derived_from<T, RenderPass>, "T must derived from RenderPass");
    Type  t             = TypeOf<T>();
    auto& render_passes = Get()->render_passes_;
    if (render_passes.contains(t))
    {
        return (T*)render_passes[t];
    }
    return nullptr;
}

template<typename T>
T* RenderPassManager::CreateRenderPass(uint32_t width, uint32_t height, const AnsiString& name)
{
    static_assert(std::derived_from<T, RenderPass>, "T must derived from RenderPass");
    Type  t             = TypeOf<T>();
    auto& render_passes = Get()->render_passes_;
    if (render_passes.contains(t))
    {
        LOG_ERROR_CATEGORY(Vulkan, L"没有新建名为{}的RenderPass,因为此RenderPass已经存在了", StringUtils::FromAnsiString(name));
        return (T*)render_passes[t];
    }
    T* new_render_pass =
        New<T>(width == 0 ? g_engine_statistics.window_size.width : width, height == 0 ? g_engine_statistics.window_size.height : height, name);
    render_passes[t] = new_render_pass;
    return new_render_pass;
}

template<typename T>
T* RenderPassManager::GetOrCreateRenderPass(uint32_t width, uint32_t height, const AnsiString& name){
    auto* rp = GetRenderPass<T>();
    if (rp) return rp;
    return CreateRenderPass<T>(width, height, name);
}

RHI_VULKAN_NAMESPACE_END
