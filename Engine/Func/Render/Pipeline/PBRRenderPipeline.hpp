//
// Created by Echo on 2025/4/2.
//

#pragma once
#include "Core/Collection/StaticArray.hpp"
#include "Core/Math/Vector.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "Core/Object/ObjectPtr.hpp"
#include "RenderPipeline.hpp"

class Texture2D;
class StaticMeshComponent;

namespace RHI
{
class Buffer;
}

class RenderTexture;
class Material;

class PBRRenderPipeline : public RenderPipeline
{
public:
    friend class PBRRenderPipelineSettingWindow;

    virtual void Render(RHI::CommandBuffer& cmd, const RenderParams& params) override;

    virtual void Build() override;

    virtual void Clean() override;

    virtual bool IsReady() const override;

    virtual void OnWindowResized(PlatformWindow* window, Int32 width, Int32 height) override;

    virtual ImGuiDrawWindow* GetSettingWindow() override;

private:
    bool mReady;
};