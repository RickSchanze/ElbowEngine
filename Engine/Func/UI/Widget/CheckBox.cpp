//
// Created by Echo on 2025/3/31.
//

#include "CheckBox.hpp"

#include "Core/Object/ObjectManager.hpp"
#include "Core/Profile.hpp"
#include "Func/Render/Helper.hpp"
#include "Func/UI/IconConstantName.hpp"
#include "Func/UI/UiManager.hpp"
#include "Func/UI/UiUtility.hpp"
#include "Platform/RHI/Commands.hpp"
#include "Text.hpp"

IMPL_REFLECTED(CheckBox) {
    return Type::Create<CheckBox>("CheckBox") | refl_helper::AddParents<Widget>() | refl_helper::AddField("text", &ThisClass::text_) |
           refl_helper::AddField("checked", &ThisClass::checked_);
}

static Rect2Df GetIconRect(Rect2Df ui_rect) {
    Rect2Df result;
    result.pos.x = ui_rect.pos.x + ui_rect.size.x - ApplyGlobalUIScale(DEFAULT_FONT_SIZE);
    result.pos.y = ui_rect.pos.y;
    result.size.x = ApplyGlobalUIScale(DEFAULT_FONT_SIZE);
    result.size.y = ApplyGlobalUIScale(DEFAULT_FONT_SIZE);
    return result;
}

CheckBox::CheckBox() {
    text_ = ObjectManager::CreateNewObject<Text>();
    ui_rect_.size.x = 0;
}

void CheckBox::Rebuild() {
    ProfileScope _(__func__);
    ValueResetScope scope{ui_rect_.size.x};
    Super::Rebuild();
    Rect2Df checkbox_icon_rect = GetIconRect(ui_rect_);
    auto write = UIManager::RequestWriteData(this, 4, 6);
    write.AddQuad(checkbox_icon_rect,
                  UIManager::GetIconAtlasUV(checked_ ? IconConstantName::CheckBox_Checked() : IconConstantName::CheckBox_UnChecked()),
                  Color::White());
    text_->SetLocation({ui_rect_.pos});
    text_->SetSize({ui_rect_.size.x - checkbox_icon_rect.size.x, ui_rect_.size.y});
    text_->Rebuild();
}

Vector2f CheckBox::GetRebuildRequiredSize() const {
    Vector2f text_size = text_->GetRebuildRequiredSize();
    text_size.x = 0; // x轴自适应父节点宽度
    return text_size;
}

void CheckBox::Draw(rhi::CommandBuffer &cmd) {
    helper::BindMaterial(cmd, GetMaterial());
    auto info = UIManager::GetWidgetBufferInfo(this);
    Assert(info, "未找到此Widget对应的Buffer, 是不是忘记调用Rebuild了?");
    cmd.Enqueue<rhi::Cmd_DrawIndexed>(info->index_count, 1, info->index_offset);
    text_->Draw(cmd);
}

void CheckBox::OnMouseButtonPressed(MouseButton button, Vector2f pos) {
    Rect2Df checkbox_icon_rect = GetIconRect(ui_rect_);
    if (UIUtility::IsRectContainsPos(checkbox_icon_rect, pos)) {
        checkbox_icon_pressed = true;
    }
}

void CheckBox::OnMouseButtonReleased(MouseButton button, Vector2f pos) {
    if (checkbox_icon_pressed) {
        checkbox_icon_pressed = false;
        SetChecked(!checked_);
    }
}

void CheckBox::OnMouseLeave() { checkbox_icon_pressed = false; }

void CheckBox::SetChecked(bool now) {
    if (now != checked_) {
        checked_ = now;
        const auto write = UIManager::RequestWriteData(this);
        write.FillQuadUV(UIManager::GetIconAtlasUV(checked_ ? IconConstantName::CheckBox_Checked() : IconConstantName::CheckBox_UnChecked()),
                         *write.vertex_buffer, *(write.vertex_buffer + 1), *(write.vertex_buffer + 2), *(write.vertex_buffer + 3));
    }
}

void CheckBox::SetText(StringView text) {
    text_->SetText(text);
    // TODO: 这里应该不需要Rebuild自己
    SetRebuildDirty();
}
