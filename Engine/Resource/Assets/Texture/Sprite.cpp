//
// Created by Echo on 25-2-18.
//

#include "Sprite.h"
#include "Texture2D.h"

#include "Func/UI/IconID.h"

#include "Resource/AssetDataBase.h"

#include GEN_HEADER("Resource.Sprite.generated.h")

using namespace resource;
GENERATED_SOURCE()

Sprite::Sprite(Texture2D *tex, core::StringView name) {
  SetTexture(tex);
  SetSpriteName(name);
}

void Sprite::SetSpriteName(core::StringView name) {

#if WITH_EDITOR || ELBOW_DEBUG
  sprite_name_ = name;
#endif
  id_ = name.GetHashCode();
}

void Sprite::SetTexture(Texture2D *tex) {
  if (tex == nullptr) {
    LOGGER.Error("Resource.Sprite", "SetTexture: 纹理为空!");
    return;
  }
  if (id_ != 0 && !tex->GetSpriteRange(id_).IsValid()) {
    LOGGER.Error("Resource.Sprite", "SetTexture: 纹理{}中不存在ID为{}的Sprite!", tex->GetName(), id_);
    return;
  }
  target_texture_ = tex;
}

SpriteRange Sprite::GetSpriteRange() const {
  Texture2D *tex = target_texture_;
  if (!tex) {
    LOGGER.Warn("Resource.Sprite", "GetSpriteRange: 纹理为空!");
    // TODO: Fallback
    return {};
  }
  return tex->GetSpriteRange(id_);
}

core::Rect2D Sprite::GetUVRange() {
  Texture2D *tex = target_texture_;
  if (!tex) {
    LOGGER.Error("Resource.Sprite", "GetUVRange: 纹理获取失败!");
    return {};
  }
  return GetUVRange(tex, id_);
}

Sprite Sprite::GetUIWhiteSprite() {
  Texture2D *tex = AssetDataBase::Load<Texture2D>("Assets/Texture/UIAtlas.png");
  Sprite range = {tex, func::ui::IconID::Name_White()};
  return range;
}

core::Rect2D Sprite::GetUVRange(Texture2D *tex, UInt64 id) {
  if (!tex)
    return {};
  // 半像素校正:
  // https://learn.microsoft.com/zh-cn/windows/win32/direct3d9/directly-mapping-texels-to-pixels?redirectedfrom=MSDN
  // https://gamedev.stackexchange.com/questions/46963/how-to-avoid-texture-bleeding-in-a-texture-atlas?newreg=0dbdf79fa0214a718ac7cd38488c56df
  core::Rect2D rect = tex->GetSpriteRange(id).range;
  rect.position += 0.5;
  rect.size.x -= 1;
  rect.size.y -= 1;
  Float w = (Float)tex->GetWidth();
  Float h = (Float)tex->GetHeight();
  rect.position.x /= w;
  rect.position.y /= h;
  rect.size.x /= w;
  rect.size.y /= h;
  return rect;
}

core::Rect2D Sprite::GetUVRange(Texture2D *tex, core::StringView name) { return GetUVRange(tex, name.GetHashCode()); }