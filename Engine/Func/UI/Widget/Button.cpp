//
// Created by Echo on 25-3-4.
//

#include "Button.h"

#include "Core/Object/ObjectRegistry.h"
#include "Func/UI/Style.h"
#include "Func/UI/UIManager.h"
#include "Func/UI/VertexHelper.h"
#include "Platform/RHI/CommandBuffer.h"
#include "Platform/RHI/Commands.h"
#include "Resource/Assets/Texture/Sprite.h"

using namespace func;
using namespace ui;
using namespace widget;
using namespace platform;
using namespace rhi;
using namespace resource;

Button::Button() {
  SetReceiveInput(true);
  padding_ = {.left = APPLY_SCALE(3), .top = APPLY_SCALE(3), .right = APPLY_SCALE(10), .bottom = APPLY_SCALE(10)};
  text_ = core::CreateNewObject<Text>();
  text_->SetText("按钮")->SetTextSize(APPLY_SCALE(20));
}

void Button::Rebuild(core::Rect2DI draw_rect) {
  // 整个巨型使用一个背景颜色
  VertexWriteData data = UIManager::RequestVertexWriteData(GetHandle(), 4, 6);
  index_offset_ = data.index_offset;
  index_size_ = 6;
  Vertex_UI left_top{};
  left_top.position.x = draw_rect.position.x;
  left_top.position.y = draw_rect.position.y + draw_rect.size.y;
  Vertex_UI right_top{};
  right_top.position.x = draw_rect.position.x + draw_rect.size.x;
  right_top.position.y = draw_rect.position.y + draw_rect.size.y;
  Vertex_UI left_bottom{};
  left_bottom.position.x = draw_rect.position.x;
  left_bottom.position.y = draw_rect.position.y;
  Vertex_UI right_bottom{};
  right_bottom.position.x = draw_rect.position.x + draw_rect.size.x;
  right_bottom.position.y = draw_rect.position.y;
  VertexHelper::FillQuadUV(Sprite::GetUIWhiteSprite().GetUVRange(), left_top, left_bottom, right_top, right_bottom);
  VertexHelper::SetQuadColor(Style::Colors::ButtonNormal(), left_top, left_bottom, right_top, right_bottom);
  VertexHelper::AppendQuad(data, left_top, left_bottom, right_top, right_bottom);
  core::Rect2DI text_draw_rect = padding_.Apply(draw_rect);
  text_->Rebuild(text_draw_rect);
  SetDirty(false);
}

void Button::Draw(CommandBuffer &cmd) {

  cmd.Enqueue<Cmd_DrawIndexed>(index_size_, 1,
                               index_offset_ DEBUG_ONLY_PARAM(core::String::Format("Button {}", GetHandle())));
  if (text_) {
    UIManager::AddDrawText(text_);
  }
}

void Button::OnMouseEnter() { IInteractable::OnMouseEnter(); }

void Button::OnMouseLeave() { IInteractable::OnMouseLeave(); }

void Button::OnMousePressed(const RespondMouses &button, Int32 x, Int32 y) {
  IInteractable::OnMousePressed(button, x, y);
}
void Button::OnMouseReleased(const RespondMouses &button, Int32 x, Int32 y) {
  IInteractable::OnMouseReleased(button, x, y);
}

core::Vector2I Button::GetBoundingSize() {
  core::Vector2I text_size = text_->GetBoundingSize();
  return {text_size.x + padding_.left + padding_.right, text_size.y + padding_.top + padding_.bottom};
}