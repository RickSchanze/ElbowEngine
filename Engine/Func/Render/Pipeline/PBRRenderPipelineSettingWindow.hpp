//
// Created by Echo on 2025/4/12.
//

#pragma once
#include "Func/UI/ImGuiDrawWindow.hpp"


class PBRRenderPipeline;
class PBRRenderPipelineSettingWindow : public ImGuiDrawWindow {
    REFLECTED_CLASS(PBRRenderPipelineSettingWindow)
public:
    void Draw() override;
    StringView GetWindowIdentity() override { return "PBRRenderPipelineSettingWindow"; }
    void SetRenderPipeline(PBRRenderPipeline* pipeline);

protected:
    void GeneratePrefilteredMapAndReplaceSkyBox() const;
    void GenerateIrradianceMapAndReplaceSkyBox() const;


    PBRRenderPipeline* pipeline_ = nullptr;
};

REGISTER_TYPE(PBRRenderPipelineSettingWindow)
