//
// Created by Echo on 25-2-22.
//

#include "WindowPanel.h"

#include "Core/Math/MathPipe.h"

#include "Func/Render/Misc.h"
#include "Func/UI/IconID.h"
#include "Func/UI/Style.h"
#include "Func/UI/VertexHelper.h"
#include "Platform/RHI/CommandBuffer.h"
#include "Platform/RHI/Commands.h"
#include "Resource/AssetDataBase.h"
#include "Resource/Assets/Font/Font.h"
#include "Resource/Assets/Material/Material.h"
#include "Text.h"
#include "Func/UI/UIManager.h"

#include GEN_HEADER("Func.WindowPanel.generated.h")
GENERATED_SOURCE()

using namespace func;
using namespace ui;
using namespace widget;
using namespace core;
using namespace platform;
using namespace rhi;
using namespace resource;

WindowPanel::WindowPanel() {
  title_ = ObjectManager::CreateNewObject<Text>();
  title_->SetName("Text_标题");
  title_->SetText("新窗口");
  title_->SetFont(Font::GetDefaultFont());
  title_->SetFontMaterial(Font::GetDefaultFontMaterial());
  title_->SetFontSize(APPLY_SCALE(title_height_));
  title_->SetColor(Color::White());
  SetWidth(APPLY_SCALE(800));
  SetHeight(APPLY_SCALE(600));
  SetName("窗口");
  UIManager::AddWindow(this);
}

void WindowPanel::Draw(CommandBuffer &cmd) {
  if (index_size_ == 0)
    return;
  Material *mat = material_;
  if (mat == nullptr) {
    LOGGER.Error("Func.UI.Panel", "Draw: Panel材质不存在, 使用默认");
    mat = AssetDataBase::Load<Material>("Assets/Material/Panel.mat");
    Assert::Require("Func.UI.Panel", mat, "Draw: 默认Panel材质不存在");
    return;
  }

  BindMaterial(cmd, mat);
  cmd.Enqueue<Cmd_DrawIndexed>(index_size_, 1, index_offset_ DEBUG_ONLY_PARAM("WindowPanelDraw"));

  title_->Draw(cmd);
}

void WindowPanel::Rebuild(Rect2DI draw_rect) {
  if (!IsDirty()) {
    return;
  }
  Panel::Rebuild(draw_rect);
  Texture2D *ui_atlas = material_->GetParam_Texture2D("atlas");
  if (ui_atlas == nullptr) {
    index_size_ = 0;
    LOGGER.Error("Func.UI.WindowPanel", "Rebuild: 窗口材质中没有atlas");
    return;
  }
  VertexWriteData data = UIManager::RequestVertexWriteData(GetHandle(), 4 * 4, 4 * 6);
  index_offset_ = data.index_offset;
  // 分两部分, 标题和内容
  // 绘制标题, 其大小根据内容调整
  Rect2DI draw_rect_ = GetDrawRect(draw_rect);
  Text *text_title_ = title_;
  Sprite white_range = Sprite::GetUIWhiteSprite();
  Rect2D white_uv_range = white_range.GetUVRange();
  // 标题栏的四个顶点
  Rect2DI title_rect{};
  title_rect.position = {draw_rect_.position.x, draw_rect_.size.y - title_height_};
  title_rect.size.x = draw_rect_.size.x;
  title_rect.size.y = title_height_;
  Vertex_UI left_top{};
  left_top.position = title_rect.LeftTop() | ToVector2;

  Vertex_UI left_bottom{};
  left_bottom.position = title_rect.LeftBottom() | ToVector2;

  Vertex_UI right_top{};
  right_top.position = title_rect.RightTop() | ToVector2;

  Vertex_UI right_bottom{};
  right_bottom.position = title_rect.RightBottom() | ToVector2;

  VertexHelper::SetQuadColor(Style::Colors::TitleBackground(), left_top, left_bottom, right_top, right_bottom);
  VertexHelper::FillQuadUV(white_uv_range, left_top, left_bottom, right_top, right_bottom);
  VertexHelper::AppendQuad(data, left_top, left_bottom, right_top, right_bottom);

  // 展开Icon的四个顶点, 以及关闭Icon的四个顶点
  core::Rect2D expanded_range = Sprite::GetUVRange(ui_atlas, expanded_ ? IconID::Expanded() : IconID::Folded());
  left_top.position.x = title_rect.position.x + 1;
  left_top.position.y = title_rect.position.y + title_height_ - 1;

  left_bottom.position.x = title_rect.position.x + 1;
  left_bottom.position.y = title_rect.position.y + 1;

  right_top.position.x = title_rect.position.x + title_height_ - 1;
  right_top.position.y = title_rect.position.y + title_height_ - 1;

  right_bottom.position.x = title_rect.position.x + title_height_ - 1;
  right_bottom.position.y = title_rect.position.y + 1;

  VertexHelper::SetQuadColor(Style::Colors::UIIconColor(), left_top, left_bottom, right_top, right_bottom);
  VertexHelper::FillQuadUV(expanded_range, left_top, left_bottom, right_top, right_bottom);
  VertexHelper::AppendQuad(data, left_top, left_bottom, right_top, right_bottom);

  // 关闭图标的四个顶点
  core::Rect2D close_range = Sprite::GetUVRange(ui_atlas, IconID::Close());
  left_top.position.x = title_rect.size.x - title_height_ + 1;
  left_top.position.y = title_rect.position.y + title_height_ - 1;

  left_bottom.position.x = title_rect.size.x - title_height_ + 1;
  left_bottom.position.y = title_rect.position.y + 1;

  right_top.position.x = title_rect.size.x - 1;
  right_top.position.y = title_rect.position.y + title_height_ - 1;

  right_bottom.position.x = title_rect.size.x - 1;
  right_bottom.position.y = title_rect.position.y + 1;

  VertexHelper::FillQuadUV(close_range, left_top, left_bottom, right_top, right_bottom);
  VertexHelper::AppendQuad(data, left_top, left_bottom, right_top, right_bottom);

  // 内容的四个顶点
  Rect2DI context_rect{};
  context_rect.position = draw_rect_.LeftBottom();
  context_rect.size.x = draw_rect_.size.x;
  context_rect.size.y = draw_rect_.size.y - title_rect.size.y;

  left_top.position = context_rect.LeftTop() | ToVector2;
  left_bottom.position = context_rect.LeftBottom() | ToVector2;
  right_top.position = context_rect.RightTop() | ToVector2;
  right_bottom.position = context_rect.RightBottom() | ToVector2;

  VertexHelper::SetQuadColor(Style::Colors::PanelBackground(), left_top, left_bottom, right_top, right_bottom);
  VertexHelper::FillQuadUV(white_uv_range, left_top, left_bottom, right_top, right_bottom);
  VertexHelper::AppendQuad(data, left_top, left_bottom, right_top, right_bottom);
  index_size_ = 4 * 6;

  Rect2DI title_text_rect;
  title_text_rect.position.x = title_rect.position.x + title_height_ + 1;
  title_text_rect.position.y = title_rect.position.y;
  title_text_rect.size.x = title_rect.size.x - title_height_ * 2 - 2;
  title_text_rect.size.y = title_rect.size.y;
  VertexHelper::TransformPosToNDCSpace(data.vertices);
  text_title_->Rebuild(title_text_rect);

  SetDirty(false);
}

void WindowPanel::SetTitle(const core::StringView &new_t) {
  if (new_t == title_->GetText()) {
    return;
  }
  title_->SetText(new_t);
  SetDirty();
}

StringView WindowPanel::GetTitle() const { return title_->GetText(); }