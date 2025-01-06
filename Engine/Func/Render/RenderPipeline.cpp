//
// Created by Echo on 25-1-2.
//

#include "RenderPipeline.h"

#include "Platform/RHI/GfxContext.h"

using namespace platform::rhi;

ImageView* func::RenderPipeline::GetBackBufferView(UInt32 current_image_index)
{
    // TODO: 区分BackBuffer为: 交换链图像, 或者一张新图像然后拷贝到交换链去(例如ImGui)
    // TODO: RenderPipeline 配置文件
    return GetGfxContextRef().GetSwapChainView(current_image_index);
}