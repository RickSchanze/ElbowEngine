//
// Created by Echo on 2025/3/29.
//

#include "Window.hpp"

#include "Func/UI/Style.hpp"
#include "Func/UI/UiManager.hpp"

IMPL_REFLECTED(Window) {
    return Type::Create<Window>("Window") | refl_helper::AddParents<Widget>() | refl_helper::AddField("title_text", &ThisClass::title_text_) |
           refl_helper::AddField("slot", &ThisClass::slot_);
}

Window::Window() {
    SetName("EmptyWindow");
    SetDisplayName("空白窗口");
    UIManager::RegisterWindow(this);
}

Window::~Window() { UIManager::UnRegisterWindow(this); }

void Window::Rebuild() {
    Super::Rebuild();
    auto write = UIManager::RequestWriteData(this, 4, 6);
    const auto color = UIManager::GetCurrentStyle().background_color;
    write.AddQuad(GetUIRect(), Rect2Df{}, color);
    Widget *s = slot_;
    Vector2f size = s->GetRebuildRequiredSize();
    s->SetLocation(ui_rect_.pos);
    s->SetSize(size);
    s->Rebuild();
}

Vector2f Window::GetRebuildRequiredSize() { return ui_rect_.size; }

void Window::SetSlotWidget(Widget *now) {
    if (now != slot_) {
        slot_ = now;
        SetRebuildDirty();
    }
}
