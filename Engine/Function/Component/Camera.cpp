/**
 * @file Camera.cpp
 * @author Echo 
 * @Date 24-5-16
 * @brief 
 */

#include "Camera.h"
#include "CoreGlobal.h"
#include "Input/Input.h"
#include "Math/Math.h"
#include "Math/MathTypes.h"
#include <glm/ext/matrix_transform.hpp>

#include "Camera.generated.h"

GENERATED_SOURCE()

FUNCTION_COMPONENT_NAMESPACE_BAGIN

Camera::Camera()
{
    SetName(L"摄像机组件");
}

void Camera::Tick(const float delta_time)
{
    Component::Tick(delta_time);
    if (enable_input_)
    {
        HandleInput();
    }
}

void Camera::BeginPlay()
{
    Super::BeginPlay();
    if (Main == nullptr)
    {
        Main = this;
    }
}

glm::mat4 Camera::GetViewMatrix() const
{
    return lookAt(transform_->location, transform_->location + transform_->GetForwardVector(), transform_->GetUpVector());
}

void Camera::SetWindowFocused(bool focused)
{
    focused_ = focused;

    // 如果foucsed（现在是按下了右键）则隐藏鼠标
    g_engine_statistics.is_hide_mouse = focused_;
}

void Camera::HandleInput()
{
    if (Input::IsKeyDown(KeyCode::MouseRight))
    {
        if (Input::IsKeyDown(KeyCode::W))
        {
            transform_->location += movement_speed_ * transform_->GetForwardVector();
        }
        if (Input::IsKeyDown(KeyCode::S))
        {
            transform_->location -= movement_speed_ * transform_->GetForwardVector();
        }
        if (Input::IsKeyDown(KeyCode::A))
        {
            transform_->location -= movement_speed_ * transform_->GetRightVector();
        }
        if (Input::IsKeyDown(KeyCode::D))
        {
            transform_->location += movement_speed_ * transform_->GetRightVector();
        }
        if (Input::IsKeyDown(KeyCode::Escape))
        {
            SetWindowFocused(false);
        }
        if (Input::IsKeyDown(KeyCode::Q))
        {
            transform_->location += movement_speed_ * transform_->GetUpVector();
        }
        if (Input::IsKeyDown(KeyCode::E))
        {
            transform_->location -= movement_speed_ * transform_->GetUpVector();
        }
        SetWindowFocused(true);
        Vector2 MouseDelta = Input::GetMouseDelta();
        MouseDelta *= mouse_sensitivity_;
        transform_->rotation.yaw += MouseDelta.x;
        transform_->rotation.pitch -= MouseDelta.y;
        transform_->rotation.pitch = Math::Clamp(transform_->rotation.pitch, -89.0f, 89.0f);
    }
    else
    {
        SetWindowFocused(false);
    }
}

FUNCTION_COMPONENT_NAMESPACE_END
