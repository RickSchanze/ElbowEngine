/**
 * @file Light.cpp
 * @author Echo 
 * @Date 24-7-29
 * @brief 
 */

#include "Light.h"

GENERATED_SOURCE()

FUNCTION_NAMESPACE_BEGIN
Comp::Light::Light() : light_type_(ELightType::Point), light_color_(Color::White()), light_intensity_(1.0f) {}
FUNCTION_NAMESPACE_END
