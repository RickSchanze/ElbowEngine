/**
 * @file ImGuiHelper.h
 * @author Echo 
 * @Date 24-7-21
 * @brief 
 */

#pragma once

#include "CoreDef.h"

#include <vulkan/vulkan_core.h>

#if USE_IMGUI
#include <imgui.h>
#endif

namespace Resource
{
class Texture;
}
class CachedString;
/**
 * ImGui wrapper
 * 项目里所有对ImGui的绘制调用走这个函数
 * 目的：实现ImGui开关
 */
class ImGuiHelper {
public:
    template <typename... Args>
    static void Text(const char* fmt, Args&&... args)
    {
#if USE_IMGUI
        ImGui::Text(fmt, Forward<Args>(args)...);
#endif
    }

    static void Text(CachedString& str);

    static bool CollapsingHeader(const char* label);

    template <typename ...Args>
    static void TextColored(const ImVec4& color, const char* fmt, Args&&... args)
    {
#if USE_IMGUI
        ImGui::TextColored(color, fmt, Forward<Args>(args)...);
#endif
    }

    static void Separator();

    static void Image(Resource::Texture* texture, int32_t width, int32_t height);

    static void SameLine();

    static void SeparatorText(const char* label);

private:
    static void RemoveAllImGuiTextures();

    static inline THashMap<Resource::Texture*, VkDescriptorSet> imgui_textuers_;
};
