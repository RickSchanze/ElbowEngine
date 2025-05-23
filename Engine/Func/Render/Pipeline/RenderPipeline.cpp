//
// Created by Echo on 2025/3/27.
//

#include "RenderPipeline.hpp"

#include <../../../../cmake-build-debug/vcpkg_installed/x64-windows/include/imgui_impl_vulkan.h>

#include "../../../Platform/RHI/CommandBuffer.hpp"
#include "../../../Platform/RHI/GfxContext.hpp"
#include "../../../Platform/Window/PlatformWindowManager.hpp"

using namespace NRHI;

void RenderPipeline::BeginImGuiFrame(NRHI::CommandBuffer &cmd, const RenderParams &render_param) {
    GetGfxContextRef().BeginImGuiFrame(cmd, render_param.CurrentImageIndex, render_param.WindowWidth, render_param.WindowHeight);
    PlatformWindowManager::BeginImGuiFrame();
    ImGui::NewFrame();
}

void RenderPipeline::EndImGuiFrame(NRHI::CommandBuffer &cmd) { GetGfxContextRef().EndImGuiFrame(cmd); }

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
