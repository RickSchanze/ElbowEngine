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
