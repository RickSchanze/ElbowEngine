//
// Created by Echo on 2025/4/7.
//

#include "UIManager.hpp"

#include "Core/Collection/Range/Range.hpp"
#include "Core/Object/ObjectManager.hpp"
#include "GlobalDockingWindow.hpp"
#include "ImGuiDrawWindow.hpp"
#include "ImGuiDrawer.hpp"
#include "ViewportWindow.hpp"

void UIManager::DrawAll() {
    auto &self = GetByRef();
    if (!self.global_docking_window_) {
        self.global_docking_window_ = ObjectManager::CreateNewObject<GlobalDockingWindow>();
    }
    self.global_docking_window_->Draw();
    for (auto &window: self.windows_ | NRange::NView::Values) {
        if (window->IsVisible())
            window->Draw();
    }
    if (self.active_viewport_window_ && self.active_viewport_window_->IsVisible()) {
        self.active_viewport_window_->Draw();
    }
}

void UIManager::AddWindow(ImGuiDrawWindow *w) {
    if (w == nullptr) {
        return;
    }
    auto &self = GetByRef();
    if (w->GetType()->IsDerivedFrom(ViewportWindow::GetStaticType())) {
        VLOG_ERROR("视口请用ActivateViewportWindow");
        return;
    }
    if (!self.windows_.Contains(w->GetWindowIdentity())) {
        self.windows_.Add(w->GetWindowIdentity(), w);
    }
}

void UIManager::RemoveWindow(ImGuiDrawWindow *w) {
    if (w == nullptr) {
        return;
    }
    auto &self = GetByRef();
    if (self.windows_.Contains(w->GetWindowIdentity())) {
        self.windows_.Remove(w->GetWindowIdentity());
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

bool UIManager::HasActiveViewportWindow() { return GetByRef().active_viewport_window_ != nullptr && GetByRef().active_viewport_window_->IsVisible(); }

ImGuiDrawWindow *UIManager::CreateOrActivateWindow(const Type *t) { return CreateOrActivateWindow(t, false); }

ImGuiDrawWindow *UIManager::CreateOrActivateWindow(const Type *t, bool silent) {
    auto &self = GetByRef();
    if (t == ViewportWindow::GetStaticType()) {
        if (self.active_viewport_window_) {
            self.active_viewport_window_->SetVisible(true);
        } else {
            self.active_viewport_window_ = CreateNewObject<ViewportWindow>();
        }
        return self.active_viewport_window_;
    } else {
        for (auto *w: self.windows_ | NRange::NView::Values) {
            if (w->GetType() == t) {
                if (!silent) {
                    w->SetVisible(true);
                }
                return w;
            }
        }
        ImGuiDrawWindow *w = static_cast<ImGuiDrawWindow *>(CreateFromType(t));
        if (silent)
            w->SetVisible(false);
        AddWindow(w);
        return w;
    }
}

ImGuiDrawWindow *UIManager::GetWindow(const Type *t) {
    auto &self = GetByRef();
    if (t == ViewportWindow::GetStaticType()) {
        return self.active_viewport_window_;
    }
    for (auto *w: self.windows_ | NRange::NView::Values) {
        if (w->GetType() == t) {
            return w;
        }
    }
    return nullptr;
}


void UIManager::Startup() {}

void UIManager::Shutdown() { global_docking_window_ = nullptr; }
