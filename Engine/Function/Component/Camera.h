/**
 * @file CameraComponent.h
 * @author Echo 
 * @Date 24-5-16
 * @brief 
 */

#pragma once
#include "ComponentBase.h"
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
    void Tick(float DeltaTime) override;

    void BeginPlay() override;

    static inline Camera* Main = nullptr;

    glm::mat4 GetViewMatrix() const;

    void SetWindowFocused(bool InFocused);

protected:
    void HandleInput();

    PROPERTY(Serialized)
    float MovementSpeed = 0.001;

    PROPERTY(Serialized, Name = EnableInput)
    bool bEnableInput = true;

    PROPERTY(Serialized, Name = DoubleClickTime)
    float mDoubleClickTime = 0.2f;

    PROPERTY(Serialized, Name = MouseSensitivity)
    float mMouseSensitivity = 0.1f;

    std::chrono::time_point<std::chrono::steady_clock> mLastClickTime;

    bool bFocused = false;
};

FUNCTION_NAMESPACE_END

// FUNCTION_NAMESPACE_END
