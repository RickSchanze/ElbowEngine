//
// Created by Echo on 25-2-18.
//

#pragma once

#include "Core/Object/Object.h"
#include "Core/Object/ObjectPtr.h"

#include "Resource.Sprite.generated.h"
#include "Texture2D.h"

namespace resource {
class Texture2D;
}
namespace resource {
struct STRUCT() Sprite {
  GENERATED_STRUCT(Sprite)

  Sprite() = default;
  Sprite(Texture2D* tex, core::StringView name);

  void SetSpriteName(core::StringView name);
  void SetTexture(Texture2D *tex);
  [[nodiscard]] SpriteRange GetSpriteRange() const;
  core::Rect2D GetUVRange();
  core::ObjectHandle GetTextureHandle() const { return target_texture_.GetHandle(); }

  static Sprite GetUIWhiteSprite();
  static core::Rect2D GetUVRange(Texture2D* tex, UInt64 id);
  static core::Rect2D GetUVRange(Texture2D* tex, core::StringView name);

private:
#if WITH_EDITOR || ELBOW_DEBUG
  core::String sprite_name_;
#endif

  PROPERTY()
  UInt64 id_ = 0;

  PROPERTY()
  core::ObjectPtr<Texture2D> target_texture_;
};
} // namespace resource
