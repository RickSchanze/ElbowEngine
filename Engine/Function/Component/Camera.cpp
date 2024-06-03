/**
 * @file Camera.cpp
 * @author Echo 
 * @Date 24-5-16
 * @brief 
 */

#include "Camera.h"
#include "EngineApplication.h"
#include "Input/Input.h"
#include "Math/Math.h"
#include "Transform.h"

#include <glm/ext/matrix_transform.hpp>

#include "Camera.generated.h"

GENERATED_SOURCE()

Function::Camera::Camera() : Component(L"摄像机组件", nullptr) {
    SetName(L"摄像机组件");
}

void Function::Camera::Tick(const float DeltaTime) {
    Component::Tick(DeltaTime);
    HandleInput();
}

void Function::Camera::BeginPlay() {
    Super::BeginPlay();
    if (Main == nullptr) {
        Main = this;
    }
}

glm::mat4 Function::Camera::GetViewMatrix() const {
    return glm::lookAt(mTransform->Position, mTransform->Position + mTransform->GetForwardVector(), mTransform->GetUpVector());
}

void Function::Camera::SetWindowFocused(bool InFocused) {
    bFocused = InFocused;
    Tool::EngineApplication::Get().SetMouseVisible(!InFocused);
}

void Function::Camera::HandleInput() {
    if (Input::IsKeyDown(KeyCode::MouseRight)) {
        if (Input::IsKeyDown(KeyCode::W)) {
            mTransform->Position += MovementSpeed * mTransform->GetForwardVector();
        }
        if (Input::IsKeyDown(KeyCode::S)) {
            mTransform->Position -= MovementSpeed * mTransform->GetForwardVector();
        }
        if (Input::IsKeyDown(KeyCode::A)) {
            mTransform->Position -= MovementSpeed * mTransform->GetRightVector();
        }
        if (Input::IsKeyDown(KeyCode::D)) {
            mTransform->Position += MovementSpeed * mTransform->GetRightVector();
        }
        if (Input::IsKeyDown(KeyCode::Escape)) {
            SetWindowFocused(false);
        }
        if (Input::IsKeyDown(KeyCode::Q)) {
            mTransform->Position += MovementSpeed * mTransform->GetUpVector();
        }
        if (Input::IsKeyDown(KeyCode::E)) {
            mTransform->Position -= MovementSpeed * mTransform->GetUpVector();
        }
        SetWindowFocused(true);
        FVector2 MouseDelta = Input::GetMouseDelta();
        MouseDelta *= mMouseSensitivity;
        mTransform->Rotation.Yaw += MouseDelta.x;
        mTransform->Rotation.Pitch -= MouseDelta.y;
        mTransform->Rotation.Pitch = FMath::Clamp(mTransform->Rotation.Pitch, -89.0f, 89.0f);
    } else {
        SetWindowFocused(false);
    }
}
