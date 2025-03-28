//
// Created by Echo on 2025/3/23.
//

#pragma once
#include "Core/Core.hpp"
#include "Core/Object/ObjectPtr.hpp"
#include "Texture2D.hpp"


class Texture2D;
struct REFLECTED() Sprite {
    REFLECTED_STRUCT(Sprite)

    Sprite() = default;
    Sprite(const Texture2D *tex, StringView name);

    void SetSpriteName(StringView name);

    void SetTexture(const Texture2D *tex);

    SpriteRange GetSpriteRange() const;

    Rect2Df GetUVRange();

    ObjectHandle GetTextureHandle() const { return target_texture_.GetHandle(); }

    static Sprite GetUIWhiteSprite();

    static Rect2Df GetUVRange(const Texture2D *tex, UInt64 id);

    static Rect2Df GetUVRange(const Texture2D *tex, const StringView name) { return GetUVRange(tex, name.GetHashCode()); }

private:
#if WITH_EDITOR || defined(ELBOW_DEBUG)
    String sprite_name_{};
#endif

    REFLECTED()
    UInt64 id_ = 0;

    REFLECTED()
    ObjectPtr<Texture2D> target_texture_;
};
