//
// Created by Echo on 2025/3/23.
//


#include "PlatformWindow.hpp"

#include "Core/Config/ConfigManager.hpp"
#include "Core/Config/CoreConfig.hpp"
#include "Platform/Config/PlatformConfig.hpp"
#include "PlatformWindowManager.hpp"

IMPL_ENUM_REFL(WindowLib) {
    return Type::Create<WindowLib>("WindowLib") | NReflHelper::AddEnumField("GLFW", std::to_underlying(WindowLib::GLFW)) |
           NReflHelper::AddEnumField("SDL3", std::to_underlying(WindowLib::SDL3)) |
           NReflHelper::AddEnumField("Count", std::to_underlying(WindowLib::Count));
}

PlatformWindow::PlatformWindow(StringView title, Int32 width, Int32 height, Int32 flags) {
    auto *core_cfg = GetConfig<CoreConfig>();
    auto *platform_cfg = GetConfig<PlatformConfig>();
    title_ = title.IsEmpty() ? core_cfg->GetAppName().ToStringView() : title;
    width_ = width == 0 ? platform_cfg->GetDefaultWindowSize().X : width;
    height_ = height == 0 ? platform_cfg->GetDefaultWindowSize().Y : height;
    flags_ = flags == -1 ? platform_cfg->GetWindowFlag() : flags;
    PlatformWindowManager::Get()->AddWindow(this);
}
