/**
 * @file Light.h
 * @author Echo 
 * @Date 24-7-29
 * @brief 
 */

#pragma once
#include "Component/Component.h"
#include "FunctionCommon.h"
#include "Math/MathTypes.h"
#include "Light.generated.h"


FUNCTION_COMPONENT_NAMESPACE_BAGIN

enum class REFL ELightType
{
    Point, // 当前只有点光源
};

/**
 * 点光源
 */
class REFL Light : public Component
{
    GENERATED_BODY(Light)

public:
    Light();

protected:
    PROPERTY(Serialized, Label="光源类型")
    ELightType light_type_;

    PROPERTY(Serialized, Label="光源颜色")
    Color light_color_;

    PROPERTY(Serialized, Label="光照强度")
    float light_intensity_;
};

FUNCTION_NAMESPACE_END
