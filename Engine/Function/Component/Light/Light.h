/**
 * @file Light.h
 * @author Echo 
 * @Date 24-7-29
 * @brief 
 */

#pragma once
#include "Component/Component.h"
#include "FunctionCommon.h"
#include "Light.generated.h"
#include "Math/MathTypes.h"
#include "Singleton/Singleton.h"


FUNCTION_COMPONENT_NAMESPACE_BAGIN

enum class REFL ELightType
{
    Point,   // 当前只有点光源
};

/**
 * 点光源
 */
class REFL Light : public Component
{
    GENERATED_BODY(Light)

public:
    Light();

    void OnEnable() override;

    void OnDisable() override;

    ELightType GetLightType() const { return light_type_; }

    Color GetLightColor() const { return light_color_; }

    float GetLightIntensity() const { return light_intensity_; }

protected:
    // TODO: 枚举类型的ImGui Display
    PROPERTY(Serialized, Label = "光源类型")
    ELightType light_type_;

    PROPERTY(Serialized, Label = "光源颜色")
    Color light_color_;

    PROPERTY(Serialized, Label = "光照强度")
    float light_intensity_;
};

class LightManager : public Singleton<LightManager>
{
public:
    void Register(Light* light);

    void UnRegister(Light* light);

    void Clear();

    TArray<Light*> GetLights() const;

private:
    // Light是组件 其生命周期由GameObject管理
    TArray<Light*> lights_;
};

FUNCTION_NAMESPACE_END
