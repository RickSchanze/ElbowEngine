//
// Created by Echo on 2025/3/27.
//

#include "Camera.hpp"

#include "CameraComponent.hpp"
#include "Platform/RHI/Buffer.hpp"
#include "Platform/RHI/GfxContext.hpp"

using namespace RHI;

void Camera::SetActive(CameraComponent *camera) {
    active_ = camera;
    // TODO: 绑定全局摄像机Uniform Buffer
}

void Camera::Tick(MilliSeconds delta_time) {
    if (auto *active = GetActive(); active != nullptr) {
        active->UpdateViewBuffer();
    }
}

void Camera::UpdateViewBuffer(const CameraShaderData &data) {
    memcpy(GetByRef().mapped_view_buffer_memory_, &data, sizeof(CameraShaderData));
}

void Camera::Startup() {
    const BufferDesc buffer_desc{sizeof(CameraShaderData), BUB_UniformBuffer, BMPB_HostCoherent | BMPB_HostVisible};
    view_buffer_ = GetGfxContext()->CreateBuffer(buffer_desc, "CameraViewBuffer");
    mapped_view_buffer_memory_ = view_buffer_->BeginWrite();
}

void Camera::Shutdown() {
    view_buffer_->EndWrite();
    view_buffer_ = nullptr;
    mapped_view_buffer_memory_ = nullptr;
}