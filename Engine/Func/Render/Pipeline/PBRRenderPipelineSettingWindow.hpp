//
// Created by Echo on 2025/4/12.
//

#pragma once
#include "Func/UI/ImGuiDrawWindow.hpp"


class Material;
class Texture2D;
class PBRRenderPipeline;
class PBRRenderPipelineSettingWindow : public ImGuiDrawWindow {
    REFLECTED_CLASS(PBRRenderPipelineSettingWindow)
public:
    void Draw() override;
    StringView GetWindowIdentity() override { return "PBRRenderPipelineSettingWindow"; }
    void SetRenderPipeline(PBRRenderPipeline *pipeline);
    void SetMeshMaterial(Material *m) { mMeshMat = m; }

protected:
    void GeneratePrefilteredMapAndApply() const;
    void GenerateIrradianceMapAndApply() const;
    void GenerateBRDFMapApply();

    Texture2D *mBRDFMap = nullptr;
    PBRRenderPipeline *mPipeline = nullptr;
    Material *mMeshMat = nullptr;
    Float mRoughness = 1;
    Float mMetallic = 1;
    Float mAO = 1;
};

REGISTER_TYPE(PBRRenderPipelineSettingWindow)
