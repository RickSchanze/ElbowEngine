/**
 * @file LightSettingWindow.cpp
 * @author Echo 
 * @Date 24-8-25
 * @brief 
 */

#include "LightSettingWindow.h"

#include "ImGui/ImGuiHelper.h"
#include "Render/Materials/Material.h"
#include "Render/Materials/SkyboxMaterial.h"

#include "LightSettingWindow.generated.h"

GENERATED_SOURCE()

WINDOW_NAMESPACE_BEGIN

LightSettingWindow::LightSettingWindow()
{
    singleton_   = true;
    name_        = L"Window_LightSettings";
    window_name_ = L"光照设置";
}

void LightSettingWindow::Draw(float delta_time)
{
    ImGuiHelper::SeparatorText(U8("天空盒设置"));
    auto* material = Function::MaterialManager::GetMaterial(L"SkyboxMaterial");
    if (material != nullptr)
    {
        auto* sky_mat = static_cast<Function::SkyboxMaterial*>(material);
        if (sky_mat->IsUsingSkyBox())
        {
            Resource::TextureCube* cube = static_cast<Resource::TextureCube*>(sky_mat->GetTextureValue("sky"));
            ImGuiHelper::Image(cube);
        }
        else
        {
            Resource::Texture* tex = sky_mat->GetTextureValue("sky");
            ImGuiHelper::Image(tex);
        }
    }
    else
    {
        ImGuiHelper::ErrorBox(U8("天空盒材质未设置"));
    }
}

WINDOW_NAMESPACE_END

