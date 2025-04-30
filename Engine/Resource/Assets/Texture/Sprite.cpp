//
// Created by Echo on 2025/3/27.
//
#include "Sprite.hpp"

#include "Resource/AssetDataBase.hpp"


Sprite::Sprite(const Texture2D *tex, const StringView name) {
    SetTexture(tex);
    SetSpriteName(name);
}

void Sprite::SetSpriteName(const StringView name) {
#if WITH_EDITOR || defined(ELBOW_DEBUG)
    sprite_name_ = name;
#endif
    id_ = name.GetHashCode();
}

void Sprite::SetTexture(const Texture2D *tex) {
    if (tex == nullptr) {
        Log(Error) << "纹理为空!";
        return;
    }
    if (id_ != 0 && !tex->GetSpriteRange(id_).IsValid()) {
        Log(Error) << String::Format("纹理{}中不存在ID为{}的Sprite!", tex->GetName(), id_);
        return;
    }
    target_texture_ = tex;
}

SpriteRange Sprite::GetSpriteRange() const {
    const Texture2D *tex = target_texture_;
    if (!tex) {
        Log(Warn) << "纹理为空!";
        // TODO: Fallback
        return {};
    }
    return tex->GetSpriteRange(id_);
}

Rect2Df Sprite::GetUVRange() {
    Texture2D *tex = target_texture_;
    if (!tex) {
        Log(Error) << "纹理获取失败!";
        return {};
    }
    return GetUVRange(tex, id_);
}

Sprite Sprite::GetUIWhiteSprite() {
    Texture2D *tex = AssetDataBase::LoadFromPath<Texture2D>("Assets/Texture/UIAtlas.png");
    Sprite range = {tex, "White"};
    return range;
}

Rect2Df Sprite::GetUVRange(const Texture2D *tex, const UInt64 id) {
    if (!tex)
        return {};
    // 半像素校正:
    // https://learn.microsoft.com/zh-cn/windows/win32/direct3d9/directly-mapping-texels-to-pixels?redirectedfrom=MSDN
    // https://gamedev.stackexchange.com/questions/46963/how-to-avoid-texture-bleeding-in-a-texture-atlas?newreg=0dbdf79fa0214a718ac7cd38488c56df
    auto rect = static_cast<Rect2Df>(tex->GetSpriteRange(id).Range);
    rect.Pos += 0.5;
    rect.Size.X -= 1;
    rect.Size.Y -= 1;
    const Float w = static_cast<Float>(tex->GetWidth());
    const Float h = static_cast<Float>(tex->GetHeight());
    rect.Pos.X /= w;
    rect.Pos.Y /= h;
    rect.Size.X /= w;
    rect.Size.Y /= h;
    return rect;
}
