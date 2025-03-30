//
// Created by Echo on 2025/3/29.
//

#include "Window.hpp"

#include "Core/Object/ObjectManager.hpp"
#include "Func/Render/Helper.hpp"
#include "Func/UI/IconConstantName.hpp"
#include "Func/UI/Style.hpp"
#include "Func/UI/UiManager.hpp"
#include "Platform/RHI/Commands.hpp"
#include "Resource/AssetDataBase.hpp"
#include "Resource/Assets/Texture/Sprite.hpp"
#include "Text.hpp"

IMPL_REFLECTED(Window) {
    return Type::Create<Window>("Window") | refl_helper::AddParents<Widget>() | refl_helper::AddField("title_text", &ThisClass::title_text_) |
           refl_helper::AddField("slot", &ThisClass::slot_);
}

Window::Window() {
    SetName("EmptyWindow");
    SetDisplayName("空白窗口");
    UIManager::RegisterWindow(this);
    title_text_ = CreateNewObject<Text>();
}

Window::~Window() { UIManager::UnRegisterWindow(this); }

void Window::Rebuild() {
    Super::Rebuild();
    // 请求三个矩形的大小 文本最后单独请求
    // ---------------------------------------------
    // |折叠标志|标题文本|---------------------------|
    // ---------------------------------------------
    // |                                            |
    // |               内容                         |
    // |                                            |
    // ---------------------------------------------|
    auto write = UIManager::RequestWriteData(this, 4 * 3, 6 * 3);
    auto ui_icon_atlas = static_cast<Texture2D *>(AssetDataBase::Load("Assets/Texture/UIAtlas.png"));
    // First 整个标题
    Rect2Df title_rect;
    title_rect.pos.x = ui_rect_.pos.x;
    title_rect.pos.y = ui_rect_.pos.y + ui_rect_.size.y - ApplyGlobalUIScale(20);
    title_rect.size.x = ui_rect_.size.x;
    title_rect.size.y = ApplyGlobalUIScale(20);
    write.AddQuad(title_rect, Sprite::GetUVRange(ui_icon_atlas, IconConstantName::PureWhite()),
                  UIManager::GetCurrentStyle().title_background_color);

    // 第三个矩形, 下面的正文
    Rect2Df content;
    content.pos = ui_rect_.pos;
    content.size.x = ui_rect_.size.x;
    content.size.y = ui_rect_.size.y - ApplyGlobalUIScale(20);
    write.AddQuad(content, Sprite::GetUVRange(ui_icon_atlas, IconConstantName::PureWhite()), UIManager::GetCurrentStyle().content_background_color);

    // 第二个矩形 折叠标志
    Rect2Df fold_rect;
    fold_rect.pos.x = ui_rect_.pos.x;
    fold_rect.pos.y = ui_rect_.pos.y + ui_rect_.size.y - ApplyGlobalUIScale(20);
    fold_rect.size.x = ApplyGlobalUIScale(20);
    fold_rect.size.y = ApplyGlobalUIScale(20);
    write.AddQuad(fold_rect, Sprite::GetUVRange(ui_icon_atlas, IconConstantName::WindowUnfold()),
                  Color::White());

    // 文本的渲染
    Text *text = title_text_;
    text->SetText(GetDisplayName());
    text->SetFontSize(ApplyGlobalUIScale(20));
    text->SetLocation({fold_rect.pos.x + fold_rect.size.x + ApplyGlobalUIScale(2), fold_rect.pos.y});
    text->SetSize(text->GetRebuildRequiredSize());
    text->Rebuild();

    Widget *s = slot_;
    Vector2f size = s->GetRebuildRequiredSize();
    s->SetLocation(ui_rect_.pos);
    s->SetSize(size);
    s->Rebuild();
}

void Window::Draw(rhi::CommandBuffer &cmd) {
    WidgetVertexIndexBufferInfo *info = UIManager::GetWidgetBufferInfo(this);
    if (info == nullptr) {
        VLOG_ERROR("未找到此Widget对应的Buffer, 是不是忘记调用Rebuild了?");
        return;
    }
    if (info->index_count == 0 || info->vertex_count == 0) {
        return;
    }
    helper::BindMaterial(cmd, GetMaterial());
    cmd.Enqueue<rhi::Cmd_DrawIndexed>(info->index_count, 1, info->index_offset);

    title_text_->Draw(cmd);
    if (slot_) {
        slot_->Draw(cmd);
    }
    cmd.Execute("Draw UI Window");
}

Vector2f Window::GetRebuildRequiredSize() { return ui_rect_.size; }

void Window::SetSlotWidget(Widget *now) {
    if (now != slot_) {
        slot_ = now;
        SetRebuildDirty();
    }
}
