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

FUNCTION_NAMESPACE_BEGIN

class REFL Camera : public Component {
    GENERATED_BODY(Camera)

    using Component::Component;

public:
    void Tick() override;
    void BeginPlay() override;
    static inline Camera* Main = nullptr;

protected:
    PROPERTY(Serialized)
    float MovementSpeed = 0.001;

    PROPERTY(Serialized, Name=EnableInput)
    bool bEnableInput = true;
};

FUNCTION_NAMESPACE_END

// FUNCTION_NAMESPACE_END
