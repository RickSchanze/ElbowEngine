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
  return tex->GetSpriteRange(id_).range / core::Vector2I{tex->GetWidth(), tex->GetHeight()};
}

Sprite Sprite::GetUIWhiteSprite() {
  Texture2D *tex = AssetDataBase::Load<Texture2D>("Assets/Texture/UIAtlas.png");
  Sprite range = {tex, func::ui::IconID::Name_White()};
  return range;
}