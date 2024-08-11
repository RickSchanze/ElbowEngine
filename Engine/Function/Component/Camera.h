/**
 * @file Camera.h
 * @author Echo 
 * @Date 24-5-16
 * @brief 
 */

#pragma once
#include "Component.h"
#include "CoreDef.h"
#include "FunctionCommon.h"
#include "Math/MathTypes.h"

#include "Camera.generated.h"


FUNCTION_COMPONENT_NAMESPACE_BAGIN

class REFL Camera : public Component
{
    GENERATED_BODY(Camera)

public:
    explicit Camera();

    void Tick(float delta_time) override;

    void BeginPlay() override;

    static inline Camera* Main = nullptr;

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;

    void SetWindowFocused(bool focused);

public:
    PROPERTY(Serialize, Label = "背景颜色")
    Color background_color = Color(0.5f, 0.5f, 0.5f, 1.0f);

protected:
    void HandleInput();

    PROPERTY(Serialized, Name = movement_speed, Label = "摄像机移动速度")
    float movement_speed_ = 0.3f;

    PROPERTY(Serialized, Name = enable_input, Label = "启用输入")
    bool enable_input_ = true;

    PROPERTY(Serialized, Name = mouse_sensitity, Label = "鼠标灵敏度")
    float mouse_sensitivity_ = 0.3f;

    bool focused_ = false;
};

FUNCTION_COMPONENT_NAMESPACE_END

// FUNCTION_NAMESPACE_END
