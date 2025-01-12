//
// Created by Echo on 25-1-12.
//

#include "Camera.h"
#include "CameraComponent.h"

using namespace func;

void Camera::SetActive(CameraComponent* camera)
{
    active_ = camera;
    // TODO: 绑定全局摄像机Uniform Buffer
}

void Camera::Tick(Millisecond delta_time)
{
    auto* active = GetActive();
    if (active != nullptr)
    {
        if (active->IsDirty())
        {
            active->UpdateViewBuffer();
        }
    }
}