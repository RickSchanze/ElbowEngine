//
// Created by Echo on 2025/3/23.
//

#pragma once
#include "Core/Core.hpp"

#include GEN_HEADER("AssetType.generated.hpp") // 最后一个且不可删除

enum class ECLASS() AssetType {
    Mesh EVALUE(Label = "网格"),
    Shader EVALUE(Label = "着色器"),
    Texture2D EVALUE(Label = "2D纹理"),
    Material EVALUE(Label = "材质"),
    Animation EVALUE(Label = "动画"),
    Audio EVALUE(Label = "音频"),
    Font EVALUE(Label = "字体"),
    Scene EVALUE(Label = "场景"),
    Prefab EVALUE(Label = "预制体"),
    Count EVALUE(Label = "超出范围")
};
