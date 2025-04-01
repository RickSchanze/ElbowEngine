//
// Created by Echo on 2025/3/29.
//

#include "Widget.hpp"

#include "Func/UI/UiManager.hpp"
#include "Resource/Assets/Material/Material.hpp"

IMPL_REFLECTED(Widget) {
    return Type::Create<Widget>("Widget") | refl_helper::AddParents<Object>() | refl_helper::AddField("visible", &Widget::visible_) |
           refl_helper::AddField("enable", &Widget::enable_) | refl_helper::AddField("receive_input", &Widget::receive_input_) |
           refl_helper::AddField("visible", &Widget::visible_) | refl_helper::AddField("rel_rect", &Widget::rel_rect_) |
           refl_helper::AddField("material", &Widget::material_) | refl_helper::AddField("parent_", &Widget::parent_);
}

void Widget::OnVisibleChanged(bool old, bool now) {}

void Widget::OnEnabledChanged(bool old, bool now) {}

Widget::Widget() { rel_rect_ = {}; }

void Widget::SetAbsoluteLocation(Vector2f loc) {
    Assert(UIManager::IsRebuilding(), "SetAbsoluteLocation只应该在Rebuild中调用.");
    abs_rect_.pos = loc;
}

void Widget::SetAbsoluteSize(Vector2f size) {
    Assert(UIManager::IsRebuilding(), "SetAbsoluteSize只应该在Rebuild中调用.");
    abs_rect_.size = size;
}

void Widget::Rebuild() { SetRebuildDirty(false); }

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
    rel_rect_.pos = loc;
    SetRebuildDirty(true);
}

void Widget::SetSize(Vector2f size) { rel_rect_.size = size; }

Material *Widget::GetMaterial() {
    if (material_ == nullptr) {
        return UIManager::GetDefaultUIMaterial();
    }
    return material_;
}

void Widget::SetMaterial(Material *mat) { material_ = mat; }

void Widget::SetParent(Widget *w) {
    Widget *old_parent = parent_;
    Widget *new_parent = w;
    if (old_parent != new_parent) {
        parent_ = new_parent;
        if (old_parent != nullptr) {
            old_parent->SetParent(nullptr);
            old_parent->SetRebuildDirty();
        }
        if (new_parent != nullptr) {
            new_parent->SetRebuildDirty();
        }
        SetRebuildDirty(true);
    }
}
