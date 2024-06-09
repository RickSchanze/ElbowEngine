/**
 * @file RenderPipeline.h
 * @author Echo 
 * @Date 24-6-9
 * @brief 
 */

#pragma once
#include "CoreDef.h"

namespace RHI::Vulkan {
class Framebuffer;
}
namespace RHI::Vulkan {
class GraphicsPipeline;
}

class RenderPipeline {
public:

protected:
    TArray<RHI::Vulkan::GraphicsPipeline*> mPipelines;

    /**
     * 一个FrameBuffer对应一个Pipeline
     * 但是同时又有多帧在并行渲染，假设当前有2帧同时渲染，有2个Pipeline则
     * mFramebuffers[CurrentFrame][CurrentPipeline]表示当前帧当前管线对应的FrameBuffer
     */
    TArray<TArray<TSharedPtr<RHI::Vulkan::Framebuffer>>> mFramebuffers;
};
