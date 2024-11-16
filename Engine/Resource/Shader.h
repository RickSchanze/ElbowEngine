/**
 * @file Shader.h
 * @author Echo 
 * @Date 24-11-10
 * @brief 
 */

#pragma once
#include "Core/Core.h"
#include "Platform/FileSystem/File.h"
#include "Interface/IResource.h"
#include "Platform/PlatformEvents.h"

#include "Core/Object/Object.h"
#include GEN_HEADER("Resource.Shader.generated.h")

namespace resource
{
class CLASS() Shader : extends core::Object, implements IResource
{
    GENERATED_CLASS(Shader)
public:
    Shader() = default;

    explicit Shader(core::StringView path) : path_(path) {}

    [[nodiscard]] core::String GetRelativePath() const override { return path_.GetRelativePath(); }
    [[nodiscard]] core::String GetAbsolutePath() const override { return path_.GetAbsolutePath(); }
    [[nodiscard]] bool         IsValid() const override;

    void Load() override;

private:
    platform::File path_;
    bool           loaded_ = false;
};
}   // namespace resource
