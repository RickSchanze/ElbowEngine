/**
 * @file Camera.cpp
 * @author Echo 
 * @Date 24-5-16
 * @brief 
 */

#include "Camera.h"
#include "CoreGlobal.h"
#include "GameObject/GameObject.h"
#include "Input/Input.h"
#include "Math/Math.h"
#include "Math/MathTypes.h"

#include "Camera.generated.h"

GENERATED_SOURCE()

namespace function::comp
{

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

Matrix4x4 Camera::GetViewMatrix() const
{
    return Math::LookAt(transform_->GetPosition(), transform_->GetPosition() + transform_->GetForwardVector(), transform_->GetUpVector());
}

Matrix4x4 Camera::GetProjectionMatrix() const
{
    auto rtn = Math::Perspective(glm::radians(45.f), 1920.f / 1080.f, near_plane, far_plane);
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
        Transform& transform = game_object_->GetTransform();
        if (Input::IsKeyDown(KeyCode::W))
        {
            transform.Translate(movement_speed_ * transform_->GetForwardVector());
        }
        if (Input::IsKeyDown(KeyCode::S))
        {
            transform.Translate(-movement_speed_ * transform_->GetForwardVector());
        }
        if (Input::IsKeyDown(KeyCode::A))
        {
            transform.Translate(-movement_speed_ * transform_->GetRightVector());
        }
        if (Input::IsKeyDown(KeyCode::D))
        {
            transform.Translate(movement_speed_ * transform_->GetRightVector());
        }
        if (Input::IsKeyDown(KeyCode::Escape))
        {
            SetWindowFocused(false);
        }
        if (Input::IsKeyDown(KeyCode::Q))
        {
            transform.Translate(movement_speed_ * Constant::UpVector);
        }
        if (Input::IsKeyDown(KeyCode::E))
        {
            transform.Translate(-movement_speed_ * Constant::UpVector);
        }
        SetWindowFocused(true);
        auto mouse_delta = Input::GetMouseDelta();
        mouse_delta *= mouse_sensitivity_;
        Rotator r;
        r.yaw           = mouse_delta.x;
        r.pitch         = -mouse_delta.y;
        Rotator new_rot = transform_->GetRotation() + r;
        if (new_rot.pitch > 89.f)
        {
            new_rot.pitch = 89.f;
        }
        if (new_rot.pitch < -89.f)
        {
            new_rot.pitch = -89.f;
        }
        transform_->SetRotation(new_rot);
    }
    else
    {
        SetWindowFocused(false);
    }
}

}
