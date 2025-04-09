//
// Created by Echo on 2025/4/7.
//

#include "ViewportWindow.hpp"

#include <imgui.h>

#include "Func/Render/Camera/Camera.hpp"
#include "Func/Render/Camera/CameraComponent.hpp"
#include "ImGuiDrawer.hpp"
#include "Platform/RHI/DescriptorSet.hpp"

IMPL_REFLECTED(ViewportWindow) { return Type::Create<ViewportWindow>("ViewportWindow") | refl_helper::AddParents<ImGuiWindow>(); }

ViewportWindow::ViewportWindow() { bound_camera_ = Camera::GetByRef().GetActive(); }

void ViewportWindow::Draw() {
    if (ImGuiDrawer::Begin("视口", &visible_)) {
        // 获取窗口的屏幕坐标原点（包括标题栏和边框）
        ImVec2 window_pos = ImGui::GetWindowPos();

        // 获取内容区域相对于窗口原点的最小/最大坐标（不包括标题栏、滚动条等）
        ImVec2 content_min = ImGui::GetWindowContentRegionMin();
        ImVec2 content_max = ImGui::GetWindowContentRegionMax();

        // 转换为屏幕坐标系
        ImVec2 content_screen_min = ImVec2(window_pos.x + content_min.x, window_pos.y + content_min.y);
        ImVec2 content_screen_max = ImVec2(window_pos.x + content_max.x, window_pos.y + content_max.y);

        // 计算内容区域的位置和尺寸
        ImVec2 content_pos = content_screen_min; // 内容区域的左上角屏幕坐标
        ImVec2 content_size = ImVec2(content_screen_max.x - content_screen_min.x, content_screen_max.y - content_screen_min.y);
        // size变化了则绑定camera的viewport
        Vector2f pos = content_pos | ToVector2f;
        Vector2f size = content_size | ToVector2f;
        if (size != size_) {
            // 重新计算摄像机的aspect_ratio和viewport
            if (!bound_camera_) {
                bound_camera_ = Camera::GetByRef().GetActive();
            }
            if (!bound_camera_) {
                VLOG_ERROR("Viewport没有绑定摄像机");
            } else {
                bound_camera_->SetAspectRatio(size.x / size.y);
            }
        }
        size_ = size;
        pos_ = pos;
        if (bound_render_texture_) {
            ImGuiDrawer::Image(*bound_render_texture_, size_);
        }
    }
    ImGui::End();
}

void ViewportWindow::BindCamera(CameraComponent *camera) {
    if (camera == nullptr) {
        return;
    }
    bound_camera_ = camera;
}

void ViewportWindow::BindRenderTexture(RenderTexture *tex) {
    if (tex == nullptr)
        return;
    if (tex == bound_render_texture_)
        return;
    bound_render_texture_ = tex;
}
