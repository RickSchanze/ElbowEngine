//
// Created by Echo on 2025/4/7.
//

#include "ViewportWindow.hpp"

#include <imgui.h>

#include "Func/Render/Camera/Camera.hpp"
#include "Func/Render/Camera/CameraComponent.hpp"
#include "ImGuiDrawer.hpp"
#include "Platform/RHI/DescriptorSet.hpp"


ViewportWindow::ViewportWindow()
{
    mBoundCamera = Camera::GetByRef().GetActive();
}

void ViewportWindow::Draw()
{
    if (ImGuiDrawer::Begin("视口", &mVisible))
    {
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
        if (size != mSize)
        {
            // 重新计算摄像机的aspect_ratio和viewport
            if (!mBoundCamera)
            {
                mBoundCamera = Camera::GetByRef().GetActive();
            }
            if (!mBoundCamera)
            {
                VLOG_ERROR("Viewport没有绑定摄像机");
            }
            else
            {
                mBoundCamera->SetAspectRatio(size.X / size.Y);
            }
        }
        mSize = size;
        mPos = pos;
        if (bound_render_texture_)
        {
            ImGuiDrawer::Image(*bound_render_texture_, mSize);
        }
        // TODO: 临时措施
        if (ImGui::IsWindowFocused())
        {
            if (ImGui::IsKeyDown(ImGuiKey_W))
            {
                mBoundCamera->SetLocation(mBoundCamera->GetLocation() + 0.1f);
            }
            if (ImGui::IsKeyDown(ImGuiKey_S))
            {
                mBoundCamera->SetLocation(mBoundCamera->GetLocation() + -0.1f);
            }
            if (ImGui::IsKeyDown(ImGuiKey_A))
            {
                mBoundCamera->SetLocation(mBoundCamera->GetLocation() + 0.1f);
            }
            if (ImGui::IsKeyDown(ImGuiKey_D))
            {
                mBoundCamera->SetLocation(mBoundCamera->GetLocation() + -0.1f);
            }
            if (ImGui::IsKeyDown(ImGuiKey_MouseLeft))
            {
                Vector3f Eluer{};
                auto &IO = ImGui::GetIO();
                Eluer.X = IO.MouseDelta.y;
                Eluer.Y = IO.MouseDelta.x;
                // mBoundCamera->Rotate(Eluer);
            }
        }
    }
    ImGui::End();
}

void ViewportWindow::BindCamera(CameraComponent *InCamera)
{
    if (InCamera == nullptr)
    {
        return;
    }
    mBoundCamera = InCamera;
}

void ViewportWindow::BindRenderTexture(RenderTexture *tex)
{
    if (tex == nullptr)
        return;
    if (tex == bound_render_texture_)
        return;
    bound_render_texture_ = tex;
}