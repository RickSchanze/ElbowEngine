//
// Created by Echo on 25-2-22.
//

#include "WindowPanel.h"

#include "Core/Math/MathPipe.h"

#include "Func/Render/Misc.h"
#include "Func/UI/Style.h"
#include "Func/UI/UIMath.h"
#include "Func/UI/VertexHelper.h"
#include "Platform/RHI/CommandBuffer.h"
#include "Platform/RHI/Commands.h"
#include "Resource/AssetDataBase.h"
#include "Resource/Assets/Font/Font.h"
#include "Resource/Assets/Material/Material.h"
#include "Text.h"

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
  title_->SetFontSize(16);
  SetWidth(800);
  SetHeight(600);
  SetName("窗口");
}

void WindowPanel::Draw(CommandBuffer &cmd) {
  Material *mat = material_;
  if (mat == nullptr) {
    LOGGER.Error("Func.UI.Panel", "Draw: Panel材质不存在, 使用默认");
    mat = AssetDataBase::Load<Material>("Assets/Material/Panel.mat");
    Assert::Require("Func.UI.Panel", mat, "Draw: 默认Panel材质不存在");
    return;
  }

  BindMaterial(cmd, mat);
  cmd.Enqueue<Cmd_DrawIndexed>(index_size_, 1, index_offset_);

  title_->Draw(cmd);
}

void WindowPanel::Rebuild(Rect2DI target_rect, Array<Vertex_UI> &vertex_buffer, Array<UInt32> &index_buffer) {
  UInt64 index_buffer_size = index_buffer.size();
  // 分两部分, 标题和内容
  // 绘制标题, 其大小根据内容调整
  Rect2DI draw_rect = GetDrawRect(target_rect);
  Text *text_title_ = title_;
  Sprite white_range = Sprite::GetUIWhiteSprite();
  Rect2D white_uv_range = white_range.GetUVRange();
  white_uv_range.position.x = 0.03126;
  white_uv_range.position.y = 0;
  white_uv_range.size.x = 0.0001;
  white_uv_range.size.y = 0.0001;
  // 标题栏的四个顶点
  Rect2DI title_rect{};
  title_rect.position = {draw_rect.position.x, draw_rect.size.y - title_height_};
  title_rect.size.x = draw_rect.size.x;
  title_rect.size.y = title_height_;
  Color title_bg_color = Style::Colors::TitleBackgroundColor();
  Vertex_UI left_top{};
  left_top.position = title_rect.LeftTop() | ToVector2;
  left_top.color = title_bg_color;

  Vertex_UI left_bottom{};
  left_bottom.position = title_rect.LeftBottom() | ToVector2;
  left_bottom.color = title_bg_color;

  Vertex_UI right_top{};
  right_top.position = title_rect.RightTop() | ToVector2;
  right_top.color = title_bg_color;

  Vertex_UI right_bottom{};
  right_bottom.position = title_rect.RightBottom() | ToVector2;
  right_bottom.color = title_bg_color;

  VertexHelper::FillQuadUV(white_uv_range, left_top, left_bottom, right_top, right_bottom);
  VertexHelper::AppendQuad(vertex_buffer, index_buffer, left_top, left_bottom, right_top, right_bottom);

  // 内容的四个顶点
  Color content_bg_color = Style::Colors::PanelBackground();
  Rect2DI context_rect{};
  context_rect.position = draw_rect.LeftBottom();
  context_rect.size.x = draw_rect.size.x;
  context_rect.size.y = draw_rect.size.y - title_rect.size.y;

  left_top.position = context_rect.LeftTop() | ToVector2;
  left_top.color = content_bg_color;

  left_bottom.position = context_rect.LeftBottom() | ToVector2;
  left_bottom.color = content_bg_color;

  right_top.position = context_rect.RightTop() | ToVector2;
  right_top.color = content_bg_color;

  right_bottom.position = context_rect.RightBottom() | ToVector2;
  right_bottom.color = content_bg_color;

  VertexHelper::AppendQuad(vertex_buffer, index_buffer, left_top, left_bottom, right_top, right_bottom);
  index_size_ = index_buffer.size() - index_buffer_size;

  text_title_->Rebuild(title_rect, vertex_buffer, index_buffer);

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