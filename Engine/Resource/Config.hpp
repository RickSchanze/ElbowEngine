//
// Created by Echo on 2025/3/23.
//

#pragma once
#include "Core/Config/IConfig.hpp"

REFLECTED(Config = "Config/Resource.cfg")
class ResourceConfig : public IConfig {
  REFLECTED_CLASS(ResourceConfig)

public:
  REFLECTED(Label = "Shader搜索路径")
  DEFINE_CFG_ITEM(Array<String>, shader_search_path, ShaderSearchPath, {"Assets/Shader"});

  REFLECTED(Label = "Shader中间产物生成路径")
  DEFINE_CFG_ITEM(String, shader_intermediate_path, ShaderIntermediatePath, {"Intermediate/Shader"});

public:
  /**
   * 先调用GetShaderIntermediatePath()，如果路径不存在则创建路径
   * @return
   */
  StringView GetValidShaderIntermediatePath() const;
};
