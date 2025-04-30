//
// Created by Echo on 2025/3/23.
//

#pragma once
#include "Core/Config/IConfig.hpp"

#include GEN_HEADER("Config.generated.hpp")

class ECLASS(Config = "Config/Resource.cfg") ResourceConfig : public IConfig
{
    GENERATED_BODY(ResourceConfig)
public:
    EFIELD()
    DEFINE_CFG_ITEM(Array<String>, mShaderSearchPath, ShaderSearchPath, {"Assets/Shader"});

    EFIELD()
    DEFINE_CFG_ITEM(String, mShaderIntermediatePath, ShaderIntermediatePath, {"Intermediate/Shader"});

public:
    /**
     * 先调用GetShaderIntermediatePath()，如果路径不存在则创建路径
     * @return
     */
    StringView GetValidShaderIntermediatePath() const;
};
