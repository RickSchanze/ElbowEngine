/**
 * @file Shader.h
 * @author Echo 
 * @Date 24-11-10
 * @brief 
 */

#pragma once
#include "Core.h"
#include "FileSystem/File.h"
#include "Interface/IResource.h"
#include "PlatformEvents.h"

#include GEN_HEADER("Resource.Shader.generated.h")

namespace resource
{
class CLASS() Shader : public IResource
{
    GENERATED_BODY(Shader)
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
