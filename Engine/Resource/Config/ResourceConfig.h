//
// Created by Echo on 24-12-25.
//

#pragma once

#include "Core/Config/IConfig.h"

namespace resource
{
class CLASS(Config = "Config/Resource.cfg") ResourceConfig : public core::IConfig
{
public:
    PROPERTY(Label = "Shader搜索路径")
    DECLARE_CONFIG_ITEM(core::Array<core::String>, shader_search_path, ShaderSearchPath, {"Assets/Shader"});
};

}   // namespace resource
