/**
 * @file Camera.cpp
 * @author Echo 
 * @Date 24-5-16
 * @brief 
 */

#include "Camera.h"
#include "Input/Input.h"
#include "Math/Math.h"
#include "Transform.h"
#include "CoreGlobal.h"
#include <glm/ext/matrix_transform.hpp>

#include "Camera.generated.h"

GENERATED_SOURCE()

Function::Camera::Camera(GameObject* InObject) : Component(L"摄像机组件", InObject) {
    SetName(L"摄像机组件");
}

void Function::Camera::Tick(const float DeltaTime) {
    Component::Tick(DeltaTime);
    if (bEnableInput) {
        HandleInput();
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

    // 如果foucsed（现在是按下了右键）则隐藏鼠标
    g_engine_statistics.is_hide_mouse = bFocused;
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
        Vector2 MouseDelta = Input::GetMouseDelta();
        MouseDelta *= mMouseSensitivity;
        mTransform->Rotation.yaw += MouseDelta.x;
        mTransform->Rotation.pitch -= MouseDelta.y;
        mTransform->Rotation.pitch = Math::Clamp(mTransform->Rotation.pitch, -89.0f, 89.0f);
    } else {
        SetWindowFocused(false);
    }
}
