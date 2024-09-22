/**
 * @file ViewportWindow.cpp
 * @author Echo 
 * @Date 24-8-17
 * @brief 
 */

#include "ViewportWindow.h"

#include "CoreEvents.h"
#include "WindowManager.h"

#include "ViewportWindow.generated.h"

GENERATED_SOURCE()

WINDOW_NAMESPACE_BEGIN

ViewportWindow::ViewportWindow()
{
    // TODO: 这里Viewport其实不应该为true
    // 也就是说应该可以有多个viewport
    // 但是目前Backbuffer只有一个
    // 如果改为false 点开多个scene viewport会导致冲突
    // 因此先设为true 等待重构
    singleton_ = true;
    OnAppWindowResized.Add(&ThisClass::OnWindowResized);
    // OnAppWindowResized.Broadcast(g_engine_statistics.window_size.width, g_engine_statistics.window_size.height);
}

void ViewportWindow::OnWindowResized(int w, int h)
{
    auto mgr     = WindowManager::Get();
    auto windows = mgr->GetWindows<ViewportWindow>();
    for (auto window: windows)
    {
        if (window->main)
        {
            OnBackbufferResize.Broadcast(window->width_, window->height_);
            return;
        }
    }
}

WINDOW_NAMESPACE_END
