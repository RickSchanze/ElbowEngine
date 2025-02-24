//
// Created by Echo on 25-1-13.
//

#pragma once
#include "Core/Singleton/MManager.h"

namespace platform::rhi {
class Buffer;
}
namespace func {

// 存储全局所有对象的实例化数据 当前包括物体的位置缩放和旋转
class GlobalObjectInstancedDataBuffer : public core::Manager<GlobalObjectInstancedDataBuffer> {
public:
  [[nodiscard]] core::ManagerLevel GetLevel() const override { return core::ManagerLevel::L8; }
  [[nodiscard]] core::StringView GetName() const override { return "GlobalObjectInstancedDataBuffer"; }

  static platform::rhi::Buffer *GetBuffer();

  void Startup() override;
  void Shutdown() override;

private:
  core::SharedPtr<platform::rhi::Buffer> buffer_;
  UInt8* mapped_memory_ = nullptr;
};

} // namespace func
