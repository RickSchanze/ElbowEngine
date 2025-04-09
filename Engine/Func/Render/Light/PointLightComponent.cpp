//
// Created by Echo on 2025/4/5.
//

#include "PointLightComponent.hpp"

#include "LightManager.hpp"

IMPL_REFLECTED(PointLightComponent) {
    return Type::Create<PointLightComponent>("PointLightComponent") | refl_helper::AddParents<SceneComponent>() |
           refl_helper::AddField("dynamic", &PointLightComponent::dynamic_);
}

PointLightComponent::PointLightComponent() {
    LightManager::AddLight(this);
}

PointLightComponent::~PointLightComponent() { LightManager::RemoveLight(this); }

void PointLightComponent::SetDynamic(bool dynamic) { this->dynamic_ = dynamic; }

void PointLightComponent::SetColor(Color color) {
    this->color_ = color;
    UpdateSelf();
}

void PointLightComponent::UpdateTransform(const Transform &parent_transform) {
    SceneComponent::UpdateTransform(parent_transform);
    UpdateSelf();
}

void PointLightComponent::OnCreated() {
    Super::OnCreated();
    LightManager::AddLight(this);
}

void PointLightComponent::SetIntensity(Float intensity) {
    SetColor(GetColor() * intensity);
}

void PointLightComponent::UpdateSelf() { LightManager::UpdateLight(this); }
