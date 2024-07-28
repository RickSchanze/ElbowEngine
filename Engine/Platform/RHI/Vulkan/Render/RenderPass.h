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

RHI_VULKAN_NAMESPACE_BEGIN

class LogicalDevice;
class Framebuffer;
class ImageView;
class Image;

struct FramebufferAttachment
{
    TUniquePtr<Image>     Image;
    TSharedPtr<ImageView> view;
};

struct RenderPassAttachmentParam
{
    vk::Format              format           = vk::Format::eUndefined;   // Undefined则会使用交换链图像格式
    vk::SampleCountFlagBits sample_count     = vk::SampleCountFlagBits::e1;
    vk::AttachmentLoadOp    load_op          = vk::AttachmentLoadOp::eClear;
    vk::AttachmentStoreOp   store_op         = vk::AttachmentStoreOp::eStore;
    vk::AttachmentLoadOp    StencilLoadOp    = vk::AttachmentLoadOp::eDontCare;
    vk::AttachmentStoreOp   StencilStoreOp   = vk::AttachmentStoreOp::eDontCare;
    vk::ImageLayout         initial_layout   = vk::ImageLayout::eUndefined;
    // 自动决定，如果SampleCount不为e1则是ColorAttachmentOptimal 否则 ePresentSrcKHR
    vk::ImageLayout         finial_layout    = vk::ImageLayout::eUndefined;
    vk::ImageLayout         reference_layout = vk::ImageLayout::eUndefined;

    vk::ImageUsageFlags image_usage;

    void Init();

    explicit RenderPassAttachmentParam(vk::ImageUsageFlags InImageUsage) { image_usage = InImageUsage; }
};

struct RenderPassAttachmentImageInfo
{
    vk::ImageUsageFlags usage;
    vk::Format          format;
    vk::ImageLayout     InitialLayout;
    vk::ImageLayout     FinalLayout;
#ifdef ELBOW_DEBUG
    const char* debug_image_name;
    const char* debug_image_view_name;
#endif
};

/**
 * 基本RenderPass 基本RenderPass包含一个ColorAttachment 一个DepthAttachment和一个Multismaple使用的Attachment
 * RenderPass同时还包含了对应的Framebuffer
 */
class RenderPass : public IRHIResource
{
public:
    bool IsValid() const;

    explicit RenderPass(const AnsiString& debug_name = "");

    ~RenderPass() override;

    void Initialize();

    // 用于创建RenderPass需要的所有Attachment
    virtual void OnCreateAttachments();

    virtual void PostInitialize() {}

    // 初始化Framebuffer和其Attachment
    virtual void SetupFramebuffer();
    virtual void CleanFrameBuffer();

    /**
     * 重置Renderpass
     * @param bDeep 为true则会 销毁RenderPass否则只销毁Framebuffer及其Attachment
     */
    virtual void Rebuild(bool bDeep);

    void Destroy() override;

    TUniquePtr<Framebuffer>& GetFrameBuffer(int32_t index) { return frame_buffers_[index]; }

    TArray<TUniquePtr<Framebuffer>>& GetFrameBuffers() { return frame_buffers_; }

    TUniquePtr<Framebuffer>& GetCurrentFrameBuffer() { return frame_buffers_[g_engine_statistics.current_image_index]; }

    vk::Framebuffer GetCurrentFrameBufferHandle();

    void Begin(vk::CommandBuffer cb, const Color& clear_color = Color::Red());
    void End(vk::CommandBuffer cb);

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
     * @param attchemnt_debug_name 这个attachment的debug name
     */
    void NewAttachment(
        RenderPassAttachmentParam& param, bool attach_to_swapchain = false, bool is_depth = false, bool is_sample_resolve = false,
        AnsiString attchemnt_debug_name = ""
    );

    virtual void CreateSubpassDescription();

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

    TArray<RenderPassAttachmentImageInfo> frame_buffer_attachment_image_infos_;
    TArray<FramebufferAttachment>         frame_buffer_attachments_;
    TArray<TUniquePtr<Framebuffer>>       frame_buffers_;

    vk::SubpassDescription subpass_;
    vk::SubpassDependency  dependency_;

#ifdef ELBOW_DEBUG
    AnsiString          render_pass_debug_name_;
    TArray<std::string> debug_image_names_;
    TArray<std::string> debug_image_view_names_;
    TArray<std::string> debug_frame_buffer_names_;
#endif
};

class RenderPassManager final : public Singleton<RenderPassManager>
{
public:
    RenderPassManager();

    template<typename T>
    static T* GetRenderPass();

    template<typename T>
    static T* CreateRenderPass(const AnsiString& name);

    template<typename T>
    static T* GetOrCreateRenderPass(const AnsiString& name);

    static void DestroyRenderPasses();

private:
    THashMap<Type, RenderPass*> render_passes_;
};

template<typename T>
T* RenderPassManager::GetRenderPass()
{
    static_assert(std::derived_from<T, RenderPass>, "T must derived from RenderPass");
    Type t = TypeOf<T>();
    auto& render_passes = Get()->render_passes_;
    if (render_passes.contains(t))
    {
        return render_passes[t];
    }
    return nullptr;
}

template<typename T>
T* RenderPassManager::CreateRenderPass(const AnsiString& name)
{
    static_assert(std::derived_from<T, RenderPass>, "T must derived from RenderPass");
    Type t = TypeOf<T>();
    auto& render_passes = Get()->render_passes_;
    if (render_passes.contains(t))
    {
        LOG_ERROR_CATEGORY(Vulkan, L"没有新建名为{}的RenderPass,因为此RenderPass已经存在了", StringUtils::FromAnsiString(name));
        return render_passes[t];
    }
    T* new_render_pass = new T(name);
    new_render_pass->Initialize();
    render_passes[t] = new_render_pass;
    return new_render_pass;
}

template<typename T>
T* RenderPassManager::GetOrCreateRenderPass(const AnsiString& name){
    auto* rp = GetRenderPass<T>();
    if (rp) return rp;
    return CreateRenderPass<T>(name);
}

RHI_VULKAN_NAMESPACE_END
