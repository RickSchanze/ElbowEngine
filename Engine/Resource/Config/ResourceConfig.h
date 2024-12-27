//
// Created by Echo on 24-12-25.
//

#pragma once

#include "Core/Config/IConfig.h"

#include GEN_HEADER("Resource.ResourceConfig.generated.h")

namespace resource
{
class CLASS(Config = "Config/Resource.cfg") ResourceConfig : public core::IConfig
{
    GENERATED_CLASS(ResourceConfig)

public:
    PROPERTY(Label = "Shader搜索路径")
    DECLARE_CONFIG_ITEM(core::Array<core::String>, shader_search_path, ShaderSearchPath, {"Assets/Shader"});

    PROPERTY(Label = "Shader中间产物生成路径")
    DECLARE_CONFIG_ITEM(core::String, shader_intermediate_path, ShaderIntermediatePath, {"Intermediate/Shader"});

public:
    /**
     * 先调用GetShaderIntermediatePath()，如果路径不存在则创建路径
     * @return
     */
    core::StringView GetValidShaderIntermediatePath() const;
};

}   // namespace resource
