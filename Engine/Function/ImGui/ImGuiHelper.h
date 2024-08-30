/**
 * @file ImGuiHelper.h
 * @author Echo 
 * @Date 24-7-21
 * @brief 
 */

#pragma once

#include "CoreDef.h"
#include "Math/MathTypes.h"
#include "RHI/Vulkan/VulkanContext.h"

#include <vulkan/vulkan_core.h>

#if USE_IMGUI
#    include <imgui.h>
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

enum EImGuiChildFlags
{
    EImGuiCF_None                   = 0,
    EImGuiCF_Border                 = 1 << 0,
    EImGuiCF_AlwaysUseWindowPadding = 1 << 1,
    EImGuiCF_ResizeX                = 1 << 2,
    EImGuiCF_ResizeY                = 1 << 3,
    EImGuiCF_AutoResizeX            = 1 << 4,
    EImGuiCF_AutoResizeY            = 1 << 5,
    EImGuiCF_AlwaysAutoResize       = 1 << 6,
    EImGuiCF_FrameStyle             = 1 << 7,
};

enum EImGuiWindowFlags
{
    EImGuiWF_None                      = 0,
    EImGuiWF_NoTitleBar                = 1 << 0,
    EImGuiWF_NoResize                  = 1 << 1,
    EImGuiWF_NoMove                    = 1 << 2,
    EImGuiWF_NoScrollbar               = 1 << 3,
    EImGuiWF_NoScrollWithMouse         = 1 << 4,
    EImGuiWF_NoCollapse                = 1 << 5,
    EImGuiWF_AlwaysAutoResize          = 1 << 6,
    EImGuiWF_NoBackground              = 1 << 7,
    EImGuiWF_NoSavedSettings           = 1 << 8,
    EImGuiWF_NoMouseInputs             = 1 << 9,
    EImGuiWF_MenuBar                   = 1 << 10,
    EImGuiWF_HorizontalScrollbar       = 1 << 11,
    EImGuiWF_NoFocusOnAppearing        = 1 << 12,
    EImGuiWF_NoBringToFrontOnFocus     = 1 << 13,
    EImGuiWF_AlwaysVerticalScrollbar   = 1 << 14,
    EImGuiWF_AlwaysHorizontalScrollbar = 1 << 15,
    EImGuiWF_NoNavInputs               = 1 << 16,
    EImGuiWF_NoNavFocus                = 1 << 17,
    EImGuiWF_UnsavedDocument           = 1 << 18,
    EImGuiWF_NoDocking                 = 1 << 19,
    EImGuiWF_NoNav                     = EImGuiWF_NoNavInputs | EImGuiWF_NoNavFocus,
    EImGuiWF_NoDecoration              = EImGuiWF_NoTitleBar | EImGuiWF_NoResize | EImGuiWF_NoScrollbar | EImGuiWF_NoCollapse,
    EImGuiWF_NoInputs                  = EImGuiWF_NoMouseInputs | EImGuiWF_NoNavInputs | EImGuiWF_NoNavFocus,
};

class ImGuiHelper
{
public:
    template<typename... Args>
    static void Text(const char* fmt, Args&&... args)
    {
#if USE_IMGUI
        ImGui::Text(fmt, Forward<Args>(args)...);
#endif
    }

    static void Text(CachedString& str);

    static bool CollapsingHeader(const char* label);

    template<typename... Args>
    static void TextColored(const ImVec4& color, const char* fmt, Args&&... args)
    {
#if USE_IMGUI
        ImGui::TextColored(color, fmt, Forward<Args>(args)...);
#endif
    }

    static void Separator();

    static void Image(Resource::Texture* texture, int32_t width, int32_t height, int32_t max_width, int32_t max_height);

    static void Image(Resource::TextureCube* texture_cube, int max_wdith = 500, int max_height = 500);

    static void Image(RHI::Vulkan::ImageView* view, RHI::Vulkan::Sampler* sampler, int width, int height, int max_width, int max_height);

    /**
     * 适应窗口宽度的显示image
     * @param texture
     * @param max_width
     * @param max_height
     */
    static void Image(Resource::Texture* texture, int max_width = 500, int max_height = 500);

    static void SameLine(float offset_from_start_x = 0, float spacing = 0);

    static void SeparatorText(const char* label);

    static void ShowDemoWindow();

    template<typename... Args>
    static void TextWrapped(const char* fmt, Args&&... args)
    {
#if USE_IMGUI
        ImGui::TextWrapped(fmt, Forward<Args>(args)...);
#endif
    }

    static void WarningBox(const char* text);

    static void ErrorBox(const char* text);

    static bool IsItemClicked();

    /**
     * new scale = old scale * scale
     * @param scale
     */
    static void PushFontScale(float scale);

    static void PopFontScale();

    static void ImageBackbuffer(int32_t width, int32_t height);

    static void ClearBackbufferDescriptorSets();

    static void BeginChild(const char* id, Vector2 size = {0, 0}, int32_t child_flags = 0, int32_t window_flags = 0);

    static void EndChild();

    static Vector2 GetContentRegionAvail();

    static void PushID(int id);

    static void PopID();

    static void PushChildWindowColor(Color col);

    static void PopChildWindowColor();

    static void PushTextColor(Color col);
    static void PushTextBackgroundColor(Color col);
    static void PopColor();

    static float GetFontSize();

    static void SetCursorPosY(float y);

    static void SetCursorPosX(float x);

    static float GetCursorPosX();

    static void SetCursorScreenPos(float x, float y);

    static void BeginGroup();

    static void EndGroup();

    static bool Button(const char* label, Vector2 size = {0, 0});

    static void SetItemTooltip(const AnsiString& tooltip);

    static Vector2 CalcTextSize(const AnsiString& str);
    static Vector2 CalcTextSize(const char* str);

    static Vector2 GetFramePadding();

    static Vector2 GetCursorScreenPos();

    static void DrawRectFilled(Vector2 min, Vector2 max, Color color, float rounding = 0);

private:
    static void RemoveAllImGuiTextures();

    static inline THashMap<RHI::Vulkan::ImageView*, VkDescriptorSet> imgui_textuers_;

    static inline TArray<VkDescriptorSet> back_image_texture_;

    static inline float old_font_scale_;
};
