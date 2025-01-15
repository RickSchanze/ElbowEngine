/**
 * @file AssetType.h
 * @author Echo 
 * @Date 24-12-4
 * @brief 
 */

#pragma once
#include "Core/Core.h"

namespace resource
{
enum class ENUM() AssetType
{
    Mesh      VALUE(Label = "网格"),
    Shader    VALUE(Label = "着色器"),
    Texture2D VALUE(Label = "2D纹理"),
    Material  VALUE(Label = "材质"),
    Animation VALUE(Label = "动画"),
    Audio     VALUE(Label = "音频"),
    Font      VALUE(Label = "字体"),
    Scene     VALUE(Label = "场景"),
    Prefab    VALUE(Label = "预制体"),
    Count     VALUE(Label = "超出范围")
};
}
