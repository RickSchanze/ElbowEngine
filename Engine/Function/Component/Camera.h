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

#include "Camera.generated.h"

#include <chrono>
#include <glm/fwd.hpp>

FUNCTION_NAMESPACE_BEGIN

class REFL Camera : public Component
{
    GENERATED_BODY(Camera)

    using Component::Component;

public:
    explicit Camera(GameObject* object);

    void Tick(float delta_time) override;

    void BeginPlay() override;

    static inline Camera* Main = nullptr;

    glm::mat4 GetViewMatrix() const;

    void SetWindowFocused(bool focused);

protected:
    void HandleInput();

    PROPERTY(Serialized, Name = movement_speed, Label = "摄像机移动速度")
    float movement_speed_ = 0.001;

    PROPERTY(Serialized, Name = enable_input, Label = "启用输入")
    bool enable_input_ = true;

    PROPERTY(Serialized, Name = mouse_sensitity, Label = "鼠标灵敏度")
    float mouse_sensitivity_ = 0.1f;

    bool focused_ = false;
};

FUNCTION_NAMESPACE_END

// FUNCTION_NAMESPACE_END
