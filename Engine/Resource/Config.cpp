//
// Created by Echo on 2025/3/27.
//

#include "Config.hpp"

#include "Core/FileSystem/Folder.hpp"
#include "Core/FileSystem/Path.hpp"

StringView ResourceConfig::GetValidShaderIntermediatePath() const {
    const StringView p = GetShaderIntermediatePath();
    if (!Path::IsExist(p)) {
        // Shader中间文件夹必须可以创建
        Folder::CreateFolder(p);
    }
    return p;
}

using namespace refl_helper;
IMPL_REFLECTED_INPLACE(ResourceConfig) {
    return Type::Create<ResourceConfig>("ResourceConfig") | AddParents<IConfig>() | Attribute(Type::ValueAttribute::Config, "Config/Resource.cfg") |
           AddField("shader_search_path", &ResourceConfig::shader_search_path) | //
           AddField("shader_intermediate_path", &ResourceConfig::shader_intermediate_path);
}
