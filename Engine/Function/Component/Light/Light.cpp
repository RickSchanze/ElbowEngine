/**
 * @file Light.cpp
 * @author Echo 
 * @Date 24-7-29
 * @brief 
 */

#include "Light.h"

#include "Utils/ContainerUtils.h"

#include "Light.generated.h"

GENERATED_SOURCE()

namespace function::comp {

Light::Light() : light_type_(ELightType::Point), light_color_(Color::White()), light_intensity_(1.0f)
{
    name_ = L"点光源";
}

void Light::OnEnable()
{
    Component::OnEnable();
    LightManager::Get()->Register(this);
}

void Light::OnDisable()
{
    LightManager::Get()->UnRegister(this);
}

void LightManager::Register(Light* light)
{
    ContainerUtils::AddUnique(lights_, light);
}

void LightManager::UnRegister(Light* light)
{
    ContainerUtils::Remove(lights_, light);
}

void LightManager::Clear()
{
    lights_.clear();
}

TArray<Light*> LightManager::GetLights() const
{
    return lights_;
}

}
