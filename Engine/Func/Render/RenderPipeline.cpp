//
// Created by Echo on 2025/3/27.
//

#include "RenderPipeline.hpp"

#include "Platform/RHI/GfxContext.hpp"

using namespace rhi;

ImageView *RenderPipeline::GetBackBufferView(UInt32 current_image_index) {
    // TODO: 区分BackBuffer为: 交换链图像, 或者一张新图像然后拷贝到交换链去(例如ImGui)
    // TODO: RenderPipeline 配置文件
    return GetGfxContextRef().GetSwapChainView(current_image_index);
}

Image *RenderPipeline::GetBackBuffer(UInt32 current_image_index) {
    // TODO: 区分BackBuffer为: 交换链图像, 或者一张新图像然后拷贝到交换链去(例如ImGui)
    // TODO: RenderPipeline 配置文件
    return GetGfxContextRef().GetSwapChainImage(current_image_index);
}
