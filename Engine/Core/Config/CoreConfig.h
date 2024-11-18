/**
 * @file CoreConfig.h
 * @author Echo 
 * @Date 24-11-16
 * @brief 
 */

#pragma once
#include "Core/Core.h"
#include "Core/Math/MathTypes.h"
#include "IConfig.h"

#include GEN_HEADER("Core.CoreConfig.generated.h")

namespace core
{
class CLASS(Config) CoreConfig : extends IConfig
{
    GENERATED_CLASS(CoreConfig)

public:
    DECLARE_CONFIG_ITEM(Color, color, Color, Color::Black());
    DECLARE_CONFIG_ITEM(Vector3, pos, Pos, {1, 2, 3});
    DECLARE_CONFIG_ITEM(Size2D, size, Size, {1920, 1080});
    DECLARE_CONFIG_ITEM(Rotator, rot, Rot, (Vector3(24, 56, 79)));
    DECLARE_CONFIG_ITEM(String, name, Name, "ElbowEngine");
    DECLARE_CONFIG_ITEM(Array<Vector3>, many_poss, ManyPos, {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});
    DECLARE_CONFIG_ITEM(HashSet<String>, many_colors, ManyColors, {"red", "green", "blue"});
    DECLARE_CONFIG_ITEM_MAP(Map<int, Rotator>, a_map, AMap, {{1, {1, 2, 3}}, {2, {4, 5, 6}}, {3, {7, 8, 9}}});
    DECLARE_CONFIG_ITEM_MAP(HashMap<String, Size2D>, a_hasmap, AHashMap, {{"one", {1, 2}}, {"two", {3, 4}}, {"three", {5, 6}}});
    DECLARE_CONFIG_ITEM(Array<Color>, many_color, ManyColor, {Color::Red(), Color::Green(), Color::Blue()});
};
}   // namespace core
