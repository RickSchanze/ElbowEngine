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

#include "Camera.generated.h"
#include "GameObject/GameObject.h"

#include <glm/ext/matrix_clip_space.hpp>

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
    return Math::LookAt(transform_->GetPosition(), transform_->GetPosition() + transform_->GetForwardVector(), transform_->GetUpVector());
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    auto rtn = glm::perspective(glm::radians(45.f), 1920.f / 1080.f, 0.1f, 1000.f);
    rtn[1][1] *= -1;
    return rtn;
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
            game_object_->Translate(movement_speed_ * transform_->GetForwardVector());
        }
        if (Input::IsKeyDown(KeyCode::S))
        {
            game_object_->Translate(-movement_speed_ * transform_->GetForwardVector());
        }
        if (Input::IsKeyDown(KeyCode::A))
        {
            game_object_->Translate(-movement_speed_ * transform_->GetRightVector());
        }
        if (Input::IsKeyDown(KeyCode::D))
        {
            game_object_->Translate(movement_speed_ * transform_->GetRightVector());
        }
        if (Input::IsKeyDown(KeyCode::Escape))
        {
            SetWindowFocused(false);
        }
        if (Input::IsKeyDown(KeyCode::Q))
        {
            game_object_->Translate(movement_speed_ * transform_->GetUpVector());
        }
        if (Input::IsKeyDown(KeyCode::E))
        {
            game_object_->Translate(-movement_speed_ * transform_->GetUpVector());
        }
        SetWindowFocused(true);
        auto mouse_delta = Input::GetMouseDelta();
        mouse_delta *= mouse_sensitivity_;
        Rotator r;
        r.yaw = mouse_delta.x;
        r.pitch = Math::Clamp(mouse_delta.y, -89.0f, 89.0f);
        transform_->Rotate(r);
    }
    else
    {
        SetWindowFocused(false);
    }
}

FUNCTION_COMPONENT_NAMESPACE_END
