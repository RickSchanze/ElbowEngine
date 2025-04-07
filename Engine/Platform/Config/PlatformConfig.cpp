//
// Created by Echo on 2025/3/25.
//
#include "PlatformConfig.hpp"

#include "Core/Core.hpp"
IMPL_REFLECTED_INPLACE(PlatformConfig) {
    return Type::Create<PlatformConfig>("PlatformConfig") |
           refl_helper::AddField("enable_multithread_render", &PlatformConfig::enable_multithread_render) |
           refl_helper::AddField("present_mode", &PlatformConfig::present_mode) |
           refl_helper::AddField("swapchain_image_count", &PlatformConfig::swapchain_image_count) |
           refl_helper::AddField("default_window_size", &PlatformConfig::default_window_size) | //
           refl_helper::AddField("api", &PlatformConfig::api) | //
           refl_helper::AddField("window_lib", &PlatformConfig::window_lib) | //
           refl_helper::AddField("window_flag", &PlatformConfig::window_flag) | //
           refl_helper::AddField("frame_count_in_flight", &PlatformConfig::frame_count_in_flight) |
#if USE_IMGUI
           refl_helper::AddField("min_imgui_image_sampler_pool_size", &PlatformConfig::min_imgui_image_sampler_pool_size) |
           refl_helper::AddField("default_imgui_font_path", &ThisClass::default_imgui_font_path) |
           refl_helper::AddField("default_imgui_font_size", &ThisClass::default_imgui_font_size) |
#endif
           refl_helper::Attribute(Type::ValueAttribute::Config, "Config/Platform/PlatformConfig.cfg");
}

bool PlatformConfig::GetValidEnableValidationLayer() const {
#ifdef ELBOW_DEBUG
    return enable_validation_layer;
#else
    return false;
#endif
}

UInt8 PlatformConfig::GetValidFrameCountInFlight() const { return std::clamp(GetFrameCountInFlight(), static_cast<UInt8>(1), static_cast<UInt8>(8)); }
