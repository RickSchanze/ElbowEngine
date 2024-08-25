/**
 * @file ImGuiHelper.h
 * @author Echo 
 * @Date 24-7-21
 * @brief 
 */

#pragma once

#include "CoreDef.h"
#include "RHI/Vulkan/VulkanContext.h"

#include <vulkan/vulkan_core.h>

#if USE_IMGUI
#include <imgui.h>
#endif

namespace Resource
{
class TextureCube;
}
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

    static void Image(Resource::Texture* texture, int32_t width, int32_t height, int32_t max_width, int32_t max_height);

    static void Image(Resource::TextureCube* texture_cube, int max_wdith = 500,int max_height = 500);

    static void Image(RHI::Vulkan::ImageView* view, RHI::Vulkan::Sampler* sampler, int width, int height, int max_width,int max_height);

    /**
     * 适应窗口宽度的显示image
     * @param texture
     * @param max_width
     * @param max_height
     */
    static void Image(Resource::Texture* texture, int max_width = 500,int max_height = 500);

    static void SameLine();

    static void SeparatorText(const char* label);

    static void ShowDemoWindow();

    template <typename ...Args>
static void TextWrapped(const char* fmt, Args&&... args)
    {
#if USE_IMGUI
        ImGui::TextWrapped(fmt, Forward<Args>(args)...);
#endif
    }

    static void WarningBox(const char* text);

    static void ErrorBox(const char* text);

    /**
     * new scale = old scale * scale
     * @param scale
     */
    static void PushFontScale(float scale);

    static void PopFontScale();

    static void ImageBackbuffer(int32_t width,int32_t height);

    static void ClearBackbufferDescriptorSets();

private:
    static void RemoveAllImGuiTextures();

    static inline THashMap<RHI::Vulkan::ImageView*, VkDescriptorSet> imgui_textuers_;

    static inline TArray<VkDescriptorSet> back_image_texture_;

    static inline float old_font_scale_;
};
