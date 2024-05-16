/**
 * @file Camera.cpp
 * @author Echo 
 * @Date 24-5-16
 * @brief 
 */

#include "Camera.h"

#include "CoreGlobal.h"
#include "EngineApplication.h"
#include "Input/Input.h"
#include "Math/Math.h"
#include "TransformComponent.h"

#include <glm/ext/matrix_transform.hpp>

void Function::Camera::Tick(const float DeltaTime) {
    Component::Tick(DeltaTime);
    if (bEnableInput) {
        if (bFocused) {
            HandleFocusedInput();
        } else {
            HandleUnfocusedInput();
        }
    }
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

void Function::Camera::HandleFocusedInput() {
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
    FVector2 MouseDelta = Input::GetMouseDelta();
    MouseDelta *= mMouseSensitivity;
    mTransform->Rotation.Yaw += MouseDelta.x;
    mTransform->Rotation.Pitch -= MouseDelta.y;
    mTransform->Rotation.Pitch = FMath::Clamp(mTransform->Rotation.Pitch, -89.0f, 89.0f);
}

void Function::Camera::HandleUnfocusedInput() {
    if (Input::IsKeyPressed(KeyCode::MouseLeft)) {
        const auto now      = std::chrono::steady_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - mLastClickTime);
        if (duration.count() <= mDoubleClickTime) {
            SetWindowFocused(true);
        } else {
            mLastClickTime = now;
        }
    }
}
