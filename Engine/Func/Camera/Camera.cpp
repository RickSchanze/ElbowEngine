//
// Created by Echo on 25-1-12.
//

#include "Camera.h"
#include "CameraComponent.h"
#include "Platform/RHI/Buffer.h"
#include "Platform/RHI/GfxContext.h"

using namespace func;
using namespace platform::rhi;

void Camera::SetActive(CameraComponent *camera) {
  active_ = camera;
  // TODO: 绑定全局摄像机Uniform Buffer
}

void Camera::Tick(Millisecond delta_time) {
  auto *active = GetActive();
  if (active != nullptr) {
    active->UpdateViewBuffer();
  }
}

void Camera::UpdateViewBuffer(const CameraShaderData &data) {
  memcpy(GetByRef().mapped_view_buffer_memory_, &data, sizeof(CameraShaderData));
}

void Camera::Startup() {
  BufferDesc buffer_desc{sizeof(CameraShaderData), BUB_UniformBuffer, BMPB_HostCoherent | BMPB_HostVisible};
  view_buffer_ = GetGfxContext()->CreateBuffer(buffer_desc, "CameraViewBuffer");
  mapped_view_buffer_memory_ = view_buffer_->BeginWrite();
}

void Camera::Shutdown() {
  view_buffer_->EndWrite();
  view_buffer_ = nullptr;
  mapped_view_buffer_memory_ = nullptr;
}