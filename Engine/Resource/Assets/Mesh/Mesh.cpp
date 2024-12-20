//
// Created by Echo on 24-12-20.
//

#include "Mesh.h"

#include "MeshMeta.h"
#include "Platform/FileSystem/Path.h"
#include "Resource/AssetDataBase.h"
#include "Resource/Logcat.h"

void resource::Mesh::PerformLoad()
{
    auto op_meta = AssetDataBase::QueryMeta<MeshMeta>(GetHandle());
    if (!op_meta)
    {
        LOGGER.Error(logcat::Resource, "加载失败, handle = {}", GetHandle());
        return;
    }
    auto& meta = *op_meta;
    auto file_path = meta.GetPath();
    if (!platform::Path::IsExist(file_path))
    {
        LOGGER.Error(logcat::Resource, "加载失败, 文件不存在, path = {}", file_path);
        return;
    }
}