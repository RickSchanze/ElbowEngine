//
// Created by Echo on 2025/4/7.
//

#include "ViewportWindow.hpp"

#include <imgui.h>

IMPL_REFLECTED(ViewportWindow) { return Type::Create<ViewportWindow>("ViewportWindow") | refl_helper::AddParents<ImGuiWindow>(); }

void ViewportWindow::Draw() {
    if (ImGui::Begin("视口")) {
        // 获取窗口的屏幕坐标原点（包括标题栏和边框）
        ImVec2 window_pos = ImGui::GetWindowPos();

        // 获取内容区域相对于窗口原点的最小/最大坐标（不包括标题栏、滚动条等）
        ImVec2 content_min = ImGui::GetWindowContentRegionMin();
        ImVec2 content_max = ImGui::GetWindowContentRegionMax();

        // 转换为屏幕坐标系
        ImVec2 content_screen_min = ImVec2(window_pos.x + content_min.x, window_pos.y + content_min.y);
        ImVec2 content_screen_max = ImVec2(window_pos.x + content_max.x, window_pos.y + content_max.y);

        // 计算内容区域的位置和尺寸
        ImVec2 content_pos = content_screen_min;  // 内容区域的左上角屏幕坐标
        ImVec2 content_size = ImVec2(content_screen_max.x - content_screen_min.x,
                                    content_screen_max.y - content_screen_min.y);
        pos_ = content_pos | ToVector2f;
        size_ = content_size | ToVector2f;
        ImGui::End();
    }
}
