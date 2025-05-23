//
// Created by Echo on 2025/4/2.
//

#pragma once
#include "Core/Collection/StaticArray.hpp"
#include "Core/Math/Vector.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "Core/Object/ObjectPtr.hpp"
#include "RenderPipeline.hpp"

#include GEN_HEADER("PBRRenderPipeline.generated.hpp")

class Texture2D;
class StaticMeshComponent;

namespace NRHI
{
class Buffer;
}

class RenderTexture;
class Material;

class ECLASS() PBRRenderPipeline : public RenderPipeline
{
    GENERATED_BODY(PBRRenderPipeline)
public:
    friend class PBRRenderPipelineSettingWindow;

    virtual void Render(NRHI::CommandBuffer& cmd, const RenderParams& params) override;

    virtual void Build() override;

    virtual void Clean() override;

    virtual bool IsReady() const override;

    virtual void OnWindowResized(PlatformWindow* window, Int32 width, Int32 height) override;

    virtual ImGuiDrawWindow* GetSettingWindow() override;

private:
    bool mReady;
};