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
#include "Func/UI/IconID.h"
#include "Func/UI/Style.h"
#include "Platform/RHI/CommandBuffer.h"
#include "Platform/RHI/Commands.h"

GENERATED_SOURCE()

using namespace func;
using namespace ui;
using namespace widget;
using namespace core;
using namespace platform::rhi;
using namespace resource;

Panel::Panel() {
  SetName("Panel");
  Texture2D *ui_atlas = AssetDataBase::Load<Texture2D>("Assets/Texture/UIAtlas.png");
  sprite_ = Sprite{ui_atlas, IconID::Name_White()};
}

void Panel::Rebuild(Rect2DI target_rect, Array<Vertex_UI> &vertex_buffer, Array<UInt32> &index_buffer) {
  Vector4I padding = GetPadding();
  auto uv_range = sprite_.GetUVRange();
  index_offset_ = index_buffer.size();
  Vertex_UI left_top{};
  left_top.position.x = target_rect.position.x + position_.x + ExtractPaddingLeft(padding);
  left_top.position.y = target_rect.position.y + position_.y + size_.y + ExtractPaddingTop(padding);
  left_top.uv.x = uv_range.position.x;
  left_top.uv.y = uv_range.position.y;
  left_top.color = Style::Colors::PanelBackground();

  Vertex_UI left_bottom{};
  left_bottom.position.x = target_rect.position.x + position_.x + ExtractPaddingLeft(padding);
  left_bottom.position.y = target_rect.position.y + position_.y + ExtractPaddingBottom(padding);
  left_bottom.uv.x = uv_range.position.x;
  left_bottom.uv.y = uv_range.position.y + uv_range.size.y;
  left_bottom.color = Style::Colors::PanelBackground();

  Vertex_UI right_top{};
  right_top.position.x = target_rect.position.x + position_.x + size_.x + ExtractPaddingRight(padding);
  right_top.position.y = target_rect.position.y + position_.y + size_.y + ExtractPaddingTop(padding);
  right_top.uv.x = uv_range.position.x + uv_range.size.x;
  right_top.uv.y = uv_range.position.y;
  right_top.color = Style::Colors::PanelBackground();

  Vertex_UI right_bottom{};
  right_bottom.position.x = target_rect.position.x + position_.x + size_.x + ExtractPaddingRight(padding);
  right_bottom.position.y = target_rect.position.y + position_.y + ExtractPaddingBottom(padding);
  right_bottom.uv.x = uv_range.position.x + uv_range.size.x;
  right_bottom.uv.y = uv_range.position.y + uv_range.size.y;
  right_bottom.color = Style::Colors::PanelBackground();

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
  dirty_ = false;
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

Panel &Panel::SetMaterial(Material *mat) {
  if (mat == material_)
    return *this;
  ObjectHandle atlas_handle = mat->GetParam_Texture2DHandle("atlas");
  if (atlas_handle != sprite_.GetTextureHandle() || atlas_handle == 0) {
    LOGGER.Error("Func.UI.Panel", "SetMaterial: Material中的atlas和Panel中的不匹配");
    return *this;
  }
  material_ = mat;
  return *this;
}

Material *Panel::GetMaterial() { return material_; }

void Panel::SetWidth(UInt32 w) {
  if (size_.x != w && w != 0) {
    SetDirty();
    size_.x = w;
  }
}

void Panel::SetHeight(UInt32 h) {
  if (size_.y != h && h != 0) {
    SetDirty();
    size_.y = h;
  }
}

Rect2DI Panel::GetDrawRect(const Rect2DI &target) const {
  // TODO: 考虑一下溢出了怎么办
  Vector4I padding = GetPadding();
  Rect2DI rect;
  Vector2I pos;
  pos.x = target.position.x + position_.x + ExtractPaddingLeft(padding);
  pos.y = target.position.y + position_.y + ExtractPaddingBottom(padding);
  rect.position = pos;
  Vector2I size;
  const UInt32 x = size_.x - ExtractPaddingLeft(padding) - ExtractPaddingRight(padding);
  const UInt32 y = size_.y - ExtractPaddingTop(padding) - ExtractPaddingBottom(padding);
  size.x = x;
  size.y = y;
  rect.size = size;
  return rect;
}
