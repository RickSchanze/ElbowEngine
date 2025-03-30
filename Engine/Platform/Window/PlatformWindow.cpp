//
// Created by Echo on 2025/3/23.
//


#include "PlatformWindow.hpp"

#include "Core/Config/ConfigManager.hpp"
#include "Core/Config/CoreConfig.hpp"
#include "Platform/Config/PlatformConfig.hpp"
#include "PlatformWindowManager.hpp"

IMPL_ENUM_REFL(WindowLib) {
    return Type::Create<WindowLib>("WindowLib") | refl_helper::AddEnumField("GLFW", std::to_underlying(WindowLib::GLFW)) |
           refl_helper::AddEnumField("SDL3", std::to_underlying(WindowLib::SDL3)) |
           refl_helper::AddEnumField("Count", std::to_underlying(WindowLib::Count));
}

EXEC_BEFORE_MAIN() { ReflManager::Get()->Register<WindowLib>(&ConstructType_WindowLib); }

PlatformWindow::PlatformWindow(StringView title, Int32 width, Int32 height, Int32 flags) {
    auto *core_cfg = GetConfig<CoreConfig>();
    auto *platform_cfg = GetConfig<PlatformConfig>();
    title_ = title.IsEmpty() ? core_cfg->GetAppName().ToStringView() : title;
    width_ = width == 0 ? platform_cfg->GetDefaultWindowSize().x : width;
    height_ = height == 0 ? platform_cfg->GetDefaultWindowSize().y : height;
    flags_ = flags == -1 ? platform_cfg->GetWindowFlag() : flags;
    PlatformWindowManager::Get()->AddWindow(this);
}
