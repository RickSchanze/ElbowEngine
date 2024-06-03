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

class REFL Camera : public Component {
    GENERATED_BODY(Camera)

    using Component::Component;

public:
    Camera();

    void Tick(float DeltaTime) override;

    void BeginPlay() override;

    static inline Camera* Main = nullptr;

    glm::mat4 GetViewMatrix() const;

    void SetWindowFocused(bool InFocused);

protected:
    void HandleInput();

    PROPERTY(Serialized, Label = "摄像机移动速度")
    float MovementSpeed = 0.001;

    PROPERTY(Serialized, Name = EnableInput, Label = "启用输入")
    bool bEnableInput = true;

    PROPERTY(Serialized, Name = MouseSensitivity, Label = "鼠标灵敏度")
    float mMouseSensitivity = 0.1f;

    bool bFocused = false;
};

FUNCTION_NAMESPACE_END

// FUNCTION_NAMESPACE_END
