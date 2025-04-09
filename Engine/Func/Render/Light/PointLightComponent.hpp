//
// Created by Echo on 2025/4/5.
//

#pragma once
#include "Func/World/SceneComponent.hpp"


class PointLightComponent : public SceneComponent {
    REFLECTED_CLASS(PointLightComponent)

    bool dynamic_ = true;
    Color color_ = Color::White();
    Float intensity_ = 1.f;

public:
    PointLightComponent();
    ~PointLightComponent() override;

    bool IsDynamic() const { return dynamic_; }
    void SetDynamic(bool dynamic);
    Color GetColor() const { return color_; }
    void SetColor(Color color);
    void UpdateTransform(const Transform &parent_transform) override;
    void OnCreated() override;

    Float GetIntensity() const { return intensity_; }
    void SetIntensity(Float intensity);

protected:
    void UpdateSelf();
};

REGISTER_TYPE(PointLightComponent)
