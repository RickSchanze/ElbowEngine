/**
 * @file SceneViewportWindow.cpp
 * @author Echo 
 * @Date 24-8-17
 * @brief 
 */

#include "SceneViewportWindow.h"

#include "CoreEvents.h"
#include "ImGui/ImGuiHelper.h"

#include "Render/RenderContext.h"
#include "SceneViewportWindow.generated.h"

GENERATED_SOURCE()

namespace tool::window {

static void OnSceneViewportWindowVisibleChanged(EWindowVisibility old, EWindowVisibility New)
{
    Function::RenderContext::has_back_buffer = New == EWindowVisibility::Visible ? true : false;
}

SceneViewportWindow::SceneViewportWindow()
{
    window_name_ = L"场景";
    name_        = L"Window_SceneWindow";

    OnVisibilityChanged.Add(OnSceneViewportWindowVisibleChanged);
}

void SceneViewportWindow::Draw(float delta_time)
{
    if (width_ != old_w_ || height_ != old_h_)
    {
        OnBackbufferResize.Broadcast(width_, height_);
        old_w_ = width_;
        old_h_ = height_;
        ImGuiHelper::ClearBackbufferDescriptorSets();
    }
    ImGuiHelper::ImageBackbuffer(width_, height_);
}

}
