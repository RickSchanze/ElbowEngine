//
// Created by Echo on 2025/3/23.
//

#pragma once
#include "Core/Core.hpp"

REFLECTED()
enum class AssetType {
    Mesh REFLECTED(Label = "网格"),
    Shader REFLECTED(Label = "着色器"),
    Texture2D REFLECTED(Label = "2D纹理"),
    Material REFLECTED(Label = "材质"),
    Animation REFLECTED(Label = "动画"),
    Audio REFLECTED(Label = "音频"),
    Font REFLECTED(Label = "字体"),
    Scene REFLECTED(Label = "场景"),
    Prefab REFLECTED(Label = "预制体"),
    Count REFLECTED(Label = "超出范围")
};
