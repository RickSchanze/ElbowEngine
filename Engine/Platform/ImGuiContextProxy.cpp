//
// Created by Echo on 2025/4/7.
//

#include "ImGuiContextProxy.hpp"
#include "Core/Async/ThreadManager.hpp"
#include "RHI/GfxContext.hpp"
#include "imgui.h"
#include "imgui_impl_vulkan.h"


void ImGuiContextProxy::CreateFontAssets() { ImGui_ImplVulkan_CreateFontsTexture(); }

void ImGuiContextProxy::DestroyFontAssets() { ImGui_ImplVulkan_DestroyFontsTexture(); }
