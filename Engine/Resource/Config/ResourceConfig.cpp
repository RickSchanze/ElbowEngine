//
// Created by Echo on 24-12-25.
//

#include "ResourceConfig.h"

#include "Platform/FileSystem/Folder.h"
#include "Platform/FileSystem/Path.h"

using namespace resource;
using namespace core;
using namespace platform;

#include GEN_HEADER("Resource.ResourceConfig.generated.h")

GENERATED_SOURCE()

StringView ResourceConfig::GetValidShaderIntermediatePath() const
{
    const StringView p = GetShaderIntermediatePath();
    if (!Path::IsExist(p))
    {
        // Shader中间文件夹必须可以创建
        Folder::CreateFolder(p);
    }
    return p;
}