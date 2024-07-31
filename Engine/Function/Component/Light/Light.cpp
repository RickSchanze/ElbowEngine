/**
 * @file Light.cpp
 * @author Echo 
 * @Date 24-7-29
 * @brief 
 */

#include "Light.h"

#include "Utils/ContainerUtils.h"

GENERATED_SOURCE()

FUNCTION_NAMESPACE_BEGIN

Comp::Light::Light() : light_type_(ELightType::Point), light_color_(Color::White()), light_intensity_(1.0f) {}

void Comp::Light::OnEnable()
{
    Component::OnEnable();
    LightManager::Get()->Register(this);
}

void Comp::Light::OnDisable()
{
    LightManager::Get()->UnRegister(this);
}

void Comp::LightManager::Register(Light* light)
{
    ContainerUtils::AddUnique(lights_, light);
}

void Comp::LightManager::UnRegister(Light* light)
{
    ContainerUtils::Remove(lights_, light);
}

void Comp::LightManager::Clear()
{
    lights_.clear();
}

TArray<Comp::Light*> Comp::LightManager::GetLights() const
{
    return lights_;
}

FUNCTION_NAMESPACE_END
