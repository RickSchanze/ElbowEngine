//
// Created by Echo on 2025/3/29.
//

#include "Widget.hpp"

#include "Func/UI/UiManager.hpp"
#include "Resource/Assets/Material/Material.hpp"

IMPL_REFLECTED(Widget) {
    return Type::Create<Widget>("Widget") | refl_helper::AddParents<Object>() | refl_helper::AddField("visible", &Widget::visible_) |
           refl_helper::AddField("enable", &Widget::enable_) | refl_helper::AddField("receive_input", &Widget::receive_input_) |
           refl_helper::AddField("visible", &Widget::visible_) | refl_helper::AddField("ui_rect", &Widget::ui_rect_) |
           refl_helper::AddField("material", &Widget::material_);
}

void Widget::OnVisibleChanged(bool old, bool now) {}

void Widget::OnEnabledChanged(bool old, bool now) {}

void Widget::Rebuild() { VLOG_FATAL("未实现!!!"); }

void Widget::SetRebuildDirty(bool dirty) { rebuild_dirty_ = dirty; }

void Widget::SetReceiveInput(bool receive_input) { receive_input_ = receive_input; }

void Widget::SetVisible(bool visible) {
    if (visible != visible_) {
        OnVisibleChanged(visible_, visible);
        visible_ = visible;
    }
}

void Widget::SetEnable(bool enable) {
    if (enable != enable_) {
        OnEnabledChanged(enable_, enable);
        enable_ = enable;
    }
}

void Widget::SetLocation(Vector2f loc) {
    ui_rect_.pos = loc;
    SetRebuildDirty(true);
}

void Widget::SetSize(Vector2f size) { ui_rect_.size = size; }

Material *Widget::GetMaterial() {
    if (material_ == nullptr) {
        return UIManager::GetDefaultUIMaterial();
    }
    return material_;
}

void Widget::SetMaterial(Material *mat) { material_ = mat; }
