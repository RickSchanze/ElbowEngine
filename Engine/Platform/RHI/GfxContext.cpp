/**
 * @file GfxContext.cpp
 * @author Echo
 * @Date 24-11-19
 * @brief
 */

#include "GfxContext.h"

#include "Core/Config/ConfigManager.h"
#include "Core/CoreGlobal.h"
#include "Core/Memory/MemoryManager.h"
#include "Platform/Config/PlatformConfig.h"
#include "Platform/PlatformLogcat.h"
#include "Vulkan/GfxContext_Vulkan.h"

namespace platform::rhi {
static GfxContext *ctx;

GfxContext::GfxContext() {
  core::MemoryManager::RequestPool(MEMORY_POOL_ID_CMD);
  Event_GfxContextPreDestroyed.AddBind(&GfxContext::PreDestroyed);
}

uint8_t GfxContext::GetSwapchainImageCount() const {
  const auto cfg = core::GetConfig<PlatformConfig>();
  return cfg->GetSwapchainImageCount();
}

core::SharedPtr<Sampler> GfxContext::GetSampler(const SamplerDesc &desc) {
  UInt64 hash = desc.GetHashCode();
  core::SharedPtr<Sampler> rtn;
  if (const auto it = sampler_pool_.find(hash); it != sampler_pool_.end()) {
    rtn = it->second;
  }
  if (rtn == nullptr) {
    rtn = CreateSampler(desc);
    sampler_pool_[hash] = rtn;
  }
  for (auto &sampler : sampler_pool_) {
    if (sampler.second.use_count() == 1) {
      sampler_pool_.erase(sampler.first);
    }
  }
  return rtn;
}

void GfxContext::Update() {
  if (cmd_allocator_) {
    cmd_allocator_->Refresh();
  } else {
    cmd_allocator_ = new core::FrameAllocator(2);
  }
}

core::FrameAllocator &GfxContext::GetCommandAllocator() {
  if (!cmd_allocator_) {
    cmd_allocator_ = new core::FrameAllocator(2);
  }
  return *cmd_allocator_;
}

void GfxContext::PreDestroyed(GfxContext *self) { self->sampler_pool_.clear(); }

GfxContext *GetGfxContext() {
  core::Assert::Require(logcat::Platform_RHI, ctx, "GfxContext not initialized");
  return ctx;
}

GfxContext &GetGfxContextRef() { return *GetGfxContext(); }

void UseGraphicsAPI(GraphicsAPI api) {
  Event_GfxContextPreInitialized.Invoke();
  switch (api) {
  case GraphicsAPI::Vulkan:
    ctx = New<vulkan::GfxContext_Vulkan>();
    break;
  default:
    core::Assert::Require(logcat::Platform_RHI, false, "Unsupported Graphics API");
  }
  Event_GfxContextPostInitialized.Invoke(ctx);
}

void ReleaseGfxContext() {
  Event_GfxContextPreDestroyed.Invoke(ctx);
  Delete(ctx);
  ctx = nullptr;
  Event_GfxContextPostDestroyed.Invoke();
}
} // namespace platform::rhi
