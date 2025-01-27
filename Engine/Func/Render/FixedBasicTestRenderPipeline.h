//
// Created by Echo on 25-1-4.
//

#pragma once
#include "Core/Base/UniquePtr.h"
#include "RenderPipeline.h"
#include "Resource/Assets/Material/Material.h"

namespace func {
class RenderTexture;
}
namespace platform::rhi {
class DescriptorSet;
}
namespace resource {
class Mesh;
}
namespace platform::rhi {
class GraphicsPipeline;
}
namespace func {

class FixedBasicTestRenderPipeline : public RenderPipeline {
public:
  void Render(platform::rhi::CommandBuffer &cmd, const RenderParams &params) override;

  void Build() override;
  void Clean() override;

  [[nodiscard]] bool IsReady() const override;

  void OnWindowResized(platform::Window *window, Int32 width, Int32 height) override;

private:
  resource::Material *material = nullptr;
  core::SharedPtr<RenderTexture> depth_target_;
  resource::Mesh *mesh_ = nullptr;
  bool ready_ = false;
};

} // namespace func
