//
// Created by Echo on 25-2-9.
//

#include "Panel.h"
#include "Core/Reflection/CtorManager.h"
#include "Core/Reflection/Reflection.h"
#include "Func/UI/UIMath.h"
#include "Platform/RHI/VertexLayout.h"
#include "Resource/AssetDataBase.h"
#include "Resource/Assets/Material/Material.h"

#include "Func.Panel.generated.h"
#include "Func/Render/Misc.h"
#include "Platform/RHI/CommandBuffer.h"
#include "Platform/RHI/Commands.h"
GENERATED_SOURCE()

using namespace func;
using namespace ui;
using namespace widget;
using namespace core;
using namespace platform::rhi;
using namespace resource;

UInt64 Panel::GetPanelBackgroundColorSpriteID() {
  static UInt64 id = core::StringView{"PanelBackgroundColor"}.GetHashCode();
  return id;
}
Panel::Panel() {
  SetName("Panel");
  Texture2D *ui_atlas = AssetDataBase::Load<Texture2D>("Assets/Texture/UIAtlas.png");
  sprite_ = Sprite{ui_atlas, "PanelBackgroundColor"};
}

void Panel::Rebuild(Rect2DI target_rect, Array<Vertex_UI> &vertex_buffer, Array<UInt32> &index_buffer) {
  Vector4I padding = GetPadding();
  auto uv_range = sprite_.GetUVRange();
  index_offset_ = index_buffer.size();
  Vertex_UI left_top{};
  left_top.position.x = target_rect.position.x + position_.x + GetPaddingLeft(padding);
  left_top.position.y = target_rect.position.y + position_.y + target_rect.size.y + GetPaddingTop(padding);
  left_top.uv.x = uv_range.position.x;
  left_top.uv.y = uv_range.position.y;

  Vertex_UI left_bottom{};
  left_bottom.position.x = target_rect.position.x + position_.x + GetPaddingLeft(padding);
  left_bottom.position.y = target_rect.position.y + position_.y + GetPaddingBottom(padding);
  left_bottom.uv.x = uv_range.position.x;
  left_bottom.uv.y = uv_range.position.y + uv_range.size.y;

  Vertex_UI right_top{};
  right_top.position.x = target_rect.position.x + position_.x + target_rect.size.x + GetPaddingRight(padding);
  right_top.position.y = target_rect.position.y + position_.y + target_rect.size.y + GetPaddingTop(padding);
  right_top.uv.x = uv_range.position.x + uv_range.size.x;
  right_top.uv.y = uv_range.position.y;

  Vertex_UI right_bottom{};
  right_bottom.position.x = target_rect.position.x + position_.x + target_rect.size.x + GetPaddingRight(padding);
  right_bottom.position.y = target_rect.position.y + position_.y + GetPaddingBottom(padding);
  right_bottom.uv.x = uv_range.position.x + uv_range.size.x;
  right_bottom.uv.y = uv_range.position.y + uv_range.size.y;

  vertex_buffer.push_back(left_top);
  vertex_buffer.push_back(left_bottom);
  vertex_buffer.push_back(right_top);
  vertex_buffer.push_back(right_bottom);

  UInt64 index_size = vertex_buffer.size();
  index_buffer.push_back(index_size - 3);
  index_buffer.push_back(index_size - 1);
  index_buffer.push_back(index_size - 2);

  index_buffer.push_back(index_size - 3);
  index_buffer.push_back(index_size - 2);
  index_buffer.push_back(index_size - 4);
}

void Panel::Draw(CommandBuffer &cmd) {
  // TODO: 合批
  Material *mat = material_;
  if (mat == nullptr) {
    LOGGER.Error("Func.UI.Panel", "Draw: Panel材质不存在, 使用默认");
    mat = AssetDataBase::Load<Material>("Assets/Material/Panel.mat");
    Assert::Require("Func.UI.Panel", mat, "Draw: 默认Panel材质不存在");
    return;
  }

  BindMaterial(cmd, mat);
  cmd.Enqueue<Cmd_DrawIndexed>(6, 1, index_offset_);
}

Rect2DI Panel::GetBoundingRect() {
  Rect2DI rtn;
  rtn.position = position_;
  rtn.size = size_;
  return rtn;
}
