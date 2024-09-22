/**
 * @file PostImageLayoutTransitionPass.h
 * @author Echo 
 * @Date 24-9-22
 * @brief 
 */

#pragma once
#include "FunctionCommon.h"
#include "RHI/Vulkan/Render/RenderPass.h"

FUNCTION_NAMESPACE_BEGIN

class PostImageLayoutTransitionPass : public RHI::Vulkan::RenderPass {

public:
    void            SetupAttachments() override;
    void            SetupFramebuffer() override;
    void            CleanFrameBuffer() override;
    vk::Framebuffer GetCurrentFramebufferHandle() override;
    void            SetupSubpassDependency() override;

    PostImageLayoutTransitionPass(uint32_t width, uint32_t height, const AnsiString& name);

public:
    TArray<RHI::Vulkan::Framebuffer*> framebuffers;
};

FUNCTION_NAMESPACE_END
