//
// Created by Echo on 24-12-25.
//

#include "Shader.h"

#include "Platform/FileSystem/Path.h"
#include "Resource/AssetDataBase.h"
#include "Resource/Logcat.h"
#include "ShaderMeta.h"

using namespace resource;

void Shader::PerformLoad()
{
    auto op_meta = AssetDataBase::QueryMeta<ShaderMeta>(GetHandle());
    if (!op_meta)
    {
        LOGGER.Error(logcat::Resource, "加载失败: handle为{}的Shader不在资产数据库", GetHandle());
        return;
    }
    auto&            meta        = *op_meta;
    core::StringView shader_path = meta.path;
    if (!platform::Path::IsExist(shader_path))
    {
        LOGGER.Error(logcat::Resource, "加载失败: 路径为{}的Shader文件不存在", shader_path);
        return;
    }
    if (!shader_path.EndsWith(".slang"))
    {
        LOGGER.Error(logcat::Resource, "加载失败: Shader必须以.slang结尾: {}", shader_path);
        return;
    }
}

bool Shader::IsLoaded() const
{
    return Asset::IsLoaded();
}