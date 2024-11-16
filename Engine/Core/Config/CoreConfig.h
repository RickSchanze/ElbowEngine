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
    PROPERTY()
    Color color = Color::Blue();

    PROPERTY()
    Vector3 pos = {1, 2, 3};

    PROPERTY()
    Rotator rot = {24, 56, 79};

    PROPERTY()
    Size2D size = {1920, 1080};

    PROPERTY()
    StringView name = "ElbowEngine";

    PROPERTY()
    Array<Vector3> many_poss = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};

    PROPERTY()
    HashSet<String> many_colors = {"red", "green", "blue"};

    PROPERTY()
    Map<int, Rotator> a_map = {{1, {1, 2, 3}}, {2, {4, 5, 6}}, {3, {7, 8, 9}}};

    PROPERTY()
    HashMap<String, Size2D> a_hasmap = {{"one", {1, 2}}, {"two", {3, 4}}, {"three", {5, 6}}};



    PROPERTY()
    Array<Color> many_color = {Color::Red(), Color::Green(), Color::Blue()};
};
}   // namespace core
