//
// Created by Echo on 25-1-12.
//

#pragma once
#include "Core/Singleton/MManager.h"
#include "Func/World/ITick.h"

namespace platform::rhi {
class DescriptorSet;
}
namespace platform::rhi {
class Buffer;
}
namespace func {
class CameraComponent;
struct CameraShaderData;
} // namespace func
namespace func {
class Camera : public core::Manager<Camera>, public ITick {
public:
  [[nodiscard]] CameraComponent *GetActive() const { return active_; }

  void SetActive(CameraComponent *camera);

  [[nodiscard]] core::ManagerLevel GetLevel() const override { return core::ManagerLevel::L6; }
  [[nodiscard]] core::StringView GetName() const override { return "Camera"; }

  void Tick(Millisecond delta_time) override;

  static void UpdateViewBuffer(const CameraShaderData &data);
  static platform::rhi::Buffer *GetViewBuffer() { return view_buffer_.get(); }

  void Startup() override;
  void Shutdown() override;

private:
  CameraComponent *active_ = nullptr;

  // 全局摄像机视图缓存
  static inline core::SharedPtr<platform::rhi::Buffer> view_buffer_ = nullptr;
};
} // namespace func
