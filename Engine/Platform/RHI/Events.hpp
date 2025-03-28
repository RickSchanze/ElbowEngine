//
// Created by Echo on 2025/3/22.
//

#pragma once
#include "Core/Event/Event.hpp"

namespace rhi {
    class GfxContext;
    struct GfxContextPreInitializedEvent : MulticastEvent<void> {};
    struct GfxContextPostInitializedEvent : MulticastEvent<void, GfxContext *> {};
    struct GfxContextPreDestroyedEvent : MulticastEvent<void, GfxContext *> {};
    struct GfxContextPostDestroyedEvent : MulticastEvent<void> {};
    struct PostProcessVulkanExtensionsEvent : Event<Array<String>, const Array<String> &> {};

    inline GfxContextPreDestroyedEvent Evt_OnGfxContextPreDestroyed;
    inline GfxContextPostDestroyedEvent Evt_OnGfxContextPostDestroyed;
    inline GfxContextPreInitializedEvent Evt_OnGfxContextPreInitialized;
    inline GfxContextPostInitializedEvent Evt_OnGfxContextPostInitialized;
    inline PostProcessVulkanExtensionsEvent Evt_PostProcessVulkanExtensions;
} // namespace rhi
