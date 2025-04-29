//
// Created by Echo on 2025/4/12.
//

#pragma once
#include "Func/UI/ImGuiDrawWindow.hpp"

#include GEN_HEADER("PBRRenderPipelineSettingWindow.generated.hpp")

class Material;
class Texture2D;
class PBRRenderPipeline;

class ECLASS() PBRRenderPipelineSettingWindow : public ImGuiDrawWindow
{
    GENERATED_BODY(PBRRenderPipelineSettingWindow)

public:
    virtual void Draw() override;

    virtual StringView GetWindowIdentity() override
    {
        return "PBRRenderPipelineSettingWindow";
    }

    void SetRenderPipeline(PBRRenderPipeline *pipeline);

    void SetMeshMaterial(Material *m)
    {
        mMeshMat = m;
    }

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