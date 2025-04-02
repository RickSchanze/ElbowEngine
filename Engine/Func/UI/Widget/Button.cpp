//
// Created by Echo on 2025/3/31.
//

#include "Button.hpp"

#include "Core/Object/ObjectManager.hpp"
#include "Core/Profile.hpp"
#include "Func/Render/Helper.hpp"
#include "Func/UI/IconConstantName.hpp"
#include "Func/UI/Style.hpp"
#include "Func/UI/UiManager.hpp"
#include "Platform/RHI/Commands.hpp"
#include "Text.hpp"


IMPL_REFLECTED(Button) {
    return Type::Create<Button>("Button") | refl_helper::AddParents<Widget>() | refl_helper::AddField("text", &ThisClass::text_) |
           refl_helper::AddField("padding", &ThisClass::padding_);
}

void Button::OnClick() { Evt_OnClicked.Invoke(); }

Color Button::SelectColor() const {
    switch (state_) {
        case ButtonState::Normal:
            return UIManager::GetCurrentStyle().button_normal_color;
        case ButtonState::Pressed:
            return UIManager::GetCurrentStyle().button_pressed_color;
        case ButtonState::Hovered:
            return UIManager::GetCurrentStyle().button_hover_color;
    }
    NeverEnter();
    return Color::White();
}

Button::Button() {
    ProfileScope _("Button::Button");
    text_ = ObjectManager::CreateNewObject<Text>();
    text_->SetText("按钮");
    text_->SetName("Button_Text");
    padding_.bottom = ApplyGlobalUIScale(10);
    padding_.top = ApplyGlobalUIScale(10);
    padding_.left = ApplyGlobalUIScale(5);
    padding_.right = ApplyGlobalUIScale(5);
}

void Button::Rebuild() {
    // 只需要绘制一个背景即可
    auto write = UIManager::RequestWriteData(this, 4, 6);
    write.AddQuad(abs_rect_, UIManager::GetIconAtlasUV(IconConstantName::PureWhite()), SelectColor());
    text_->SetAbsoluteLocation({abs_rect_.pos.x + padding_.left, abs_rect_.pos.y + padding_.bottom});
    // TODO: 正确性
    text_->SetAbsoluteSize({abs_rect_.size.x - padding_.left - padding_.right, abs_rect_.size.y - padding_.top - padding_.bottom});
    text_->Rebuild();
}

void Button::Draw(rhi::CommandBuffer &cmd) {
    // TODO: 合批
    ProfileScope _(__func__);
    helper::BindMaterial(cmd, GetMaterial());
    auto info = UIManager::GetWidgetBufferInfo(this);
    Assert(info, "未找到此Widget对应的Buffer, 是不是忘记调用Rebuild了?");
    cmd.DrawIndexed(info->index_count, 1, info->index_offset);
    text_->Draw(cmd);
}

Vector2f Button::GetRebuildRequiredSize() const {
    ProfileScope _(__func__);
    Vector2f size = text_->GetRebuildRequiredSize();
    size.x += (padding_.left + padding_.right);
    size.y += (padding_.top + padding_.bottom);
    return size;
}

void Button::OnMouseEnter() {
    ProfileScope _(__func__);
    state_ = ButtonState::Hovered;
    auto write = UIManager::RequestWriteData(this);
    write.SetQuadColor(UIManager::GetCurrentStyle().button_hover_color, *write.vertex_buffer, *(write.vertex_buffer + 1), *(write.vertex_buffer + 2),
                       *(write.vertex_buffer + 3));
}

void Button::OnMouseLeave() {
    ProfileScope _(__func__);
    state_ = ButtonState::Normal;
    auto write = UIManager::RequestWriteData(this);
    write.SetQuadColor(UIManager::GetCurrentStyle().button_normal_color, *write.vertex_buffer, *(write.vertex_buffer + 1), *(write.vertex_buffer + 2),
                       *(write.vertex_buffer + 3));
}

void Button::OnMouseButtonPressed(MouseButton button, Vector2f pos) {
    ProfileScope _(__func__);
    state_ = ButtonState::Pressed;
    auto write = UIManager::RequestWriteData(this);
    write.SetQuadColor(UIManager::GetCurrentStyle().button_pressed_color, *write.vertex_buffer, *(write.vertex_buffer + 1),
                       *(write.vertex_buffer + 2), *(write.vertex_buffer + 3));
}

void Button::OnMouseButtonReleased(MouseButton button, Vector2f pos) {
    ProfileScope _(__func__);
    state_ = ButtonState::Hovered;
    auto write = UIManager::RequestWriteData(this);
    write.SetQuadColor(UIManager::GetCurrentStyle().button_hover_color, *write.vertex_buffer, *(write.vertex_buffer + 1), *(write.vertex_buffer + 2),
                       *(write.vertex_buffer + 3));
    OnClick();
}
