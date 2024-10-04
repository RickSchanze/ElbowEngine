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

namespace tool::window {

LightSettingWindow::LightSettingWindow()
{
    singleton_   = true;
    name_        = L"Window_LightSettings";
    window_name_ = L"光照设置";
}

void LightSettingWindow::Draw(float delta_time)
{
    ImGuiHelper::SeparatorText(U8("天空盒设置"));
    auto* material = function::MaterialManager::GetMaterial(L"SkyboxMaterial");
    if (material != nullptr && material->GetTextureValue("sky") != nullptr && !material->GetTextureValue("sky")->IsDefaultLackTexture())
    {
        auto* sky_mat = static_cast<function::SkyboxMaterial*>(material);
        if (sky_mat->IsUsingSkyBox())
        {
            res::TextureCube* cube = static_cast<res::TextureCube*>(sky_mat->GetTextureValue("sky"));
            ImGuiHelper::Image(cube);
        }
        else
        {
            res::Texture* tex = sky_mat->GetTextureValue("sky");
            ImGuiHelper::Image(tex);
        }
    }
    else
    {
        ImGuiHelper::ErrorBox(U8("天空盒材质未设置"));
    }
}

}

