//
// Created by Echo on 2025/4/7.
//

#include "UIManager.hpp"

#include "Core/Collection/Range/Range.hpp"
#include "Core/Object/ObjectManager.hpp"
#include "GlobalDockingWindow.hpp"
#include "ImGuiWindow.hpp"
#include "ViewportWindow.hpp"

void UIManager::DrawAll() {
    auto &self = GetByRef();
    if (!self.global_docking_window_) {
        self.global_docking_window_ = ObjectManager::CreateNewObject<GlobalDockingWindow>();
    }
    self.global_docking_window_->Draw();
    for (auto &window: self.windows_ | range::view::Values) {
        if (window->IsVisible())
            window->Draw();
    }
    if (self.active_viewport_window_ && self.active_viewport_window_->IsVisible()) {
        self.active_viewport_window_->Draw();
    }
}

void UIManager::AddWindow(ImGuiWindow *w) {
    if (w == nullptr) {
        return;
    }
    auto &self = GetByRef();
    if (w->GetType()->IsDerivedFrom(ViewportWindow::GetStaticType())) {
        VLOG_ERROR("视口请用ActivateViewportWindow");
        return;
    }
    if (!self.windows_.Contains(w->GetWindowTitle())) {
        self.windows_.Add(w->GetWindowTitle(), w);
    }
}

void UIManager::RemoveWindow(ImGuiWindow *w) {
    if (w == nullptr) {
        return;
    }
    auto &self = GetByRef();
    if (self.windows_.Contains(w->GetWindowTitle())) {
        self.windows_.Remove(w->GetWindowTitle());
    }
    if (w == self.active_viewport_window_) {
        self.active_viewport_window_ = nullptr;
    }
}

ViewportWindow *UIManager::GetActiveViewportWindow() { return GetByRef().active_viewport_window_; }

void UIManager::ActivateViewportWindow() {
    // 当前只能有一个viewport
    // TODO: 多viewport支持
    auto &self = GetByRef();
    if (!self.active_viewport_window_) {
        self.active_viewport_window_ = ObjectManager::CreateNewObject<ViewportWindow>();
    } else {
        self.active_viewport_window_->SetVisible(true);
    }
}


void UIManager::Startup() {}

void UIManager::Shutdown() { global_docking_window_ = nullptr; }
