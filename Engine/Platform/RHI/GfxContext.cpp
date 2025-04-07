//
// Created by Echo on 2025/3/22.
//

#include "GfxContext.hpp"
#include "Vulkan/GfxContext.hpp"

#include <imgui.h>

#include "Core/Config/ConfigManager.hpp"
#include "Core/Memory/FrameAllocator.hpp"
#include "Events.hpp"
#include "Image.hpp"
#include "Platform/Config/PlatformConfig.hpp"

using namespace rhi;

static GfxContext *ctx;

GfxContext::GfxContext() { Evt_OnGfxContextPreDestroyed.AddBind(&GfxContext::PreDestroyed); }

GfxContext::~GfxContext() { Delete(cmd_allocator_); }

UInt8 GfxContext::GetSwapchainImageCount() const {
    const auto cfg = GetConfig<PlatformConfig>();
    return cfg->GetSwapchainImageCount();
}

SharedPtr<Sampler> GfxContext::GetSampler(const SamplerDesc &desc) {
    UInt64 hash = desc.GetHashCode();
    SharedPtr<Sampler> rtn;
    if (const auto it = sampler_pool_.Find(hash); it != sampler_pool_.end()) {
        rtn = it->second;
    }
    if (rtn == nullptr) {
        rtn = CreateSampler(desc);
        sampler_pool_[hash] = rtn;
    }
    // TODO: 这块是有问题的, 迭代其循环里进行了Remove
    for (auto &sampler: sampler_pool_) {
        if (sampler.second.use_count() == 1) {
            sampler_pool_.Remove(sampler.first);
        }
    }
    return rtn;
}

void GfxContext::Update() {
    if (cmd_allocator_) {
        cmd_allocator_->Refresh();
    } else {
        cmd_allocator_ = New<FrameAllocator>(2);
    }
}

FrameAllocator &GfxContext::GetCommandAllocator() {
    if (!cmd_allocator_) {
        cmd_allocator_ = New<FrameAllocator>(2);
    }
    return *cmd_allocator_;
}


void GfxContext::PreDestroyed(GfxContext *self) { self->sampler_pool_.Clear(); }

GfxContext *rhi::GetGfxContext() {
    Assert(ctx, "GfxContext not initialized");
    return ctx;
}

GfxContext &rhi::GetGfxContextRef() { return *GetGfxContext(); }

void rhi::UseGraphicsAPI(GraphicsAPI api) {
    Evt_OnGfxContextPreInitialized.Invoke();
    switch (api) {
        case GraphicsAPI::Vulkan:
            ctx = New<GfxContext_Vulkan>();
            break;
        default:
            Assert(false, "Unsupported Graphics API");
    }
    Evt_OnGfxContextPostInitialized.Invoke(ctx);
}

void rhi::ReleaseGfxContext() {
    Evt_OnGfxContextPreDestroyed.Invoke(ctx);
    Delete(ctx);
    ctx = nullptr;
    Evt_OnGfxContextPostDestroyed.Invoke();
}
