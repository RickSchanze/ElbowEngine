/**
 * @file AssetDataBase.cpp
 * @author Echo 
 * @Date 24-12-4
 * @brief 
 */

#include "AssetDataBase.h"

#include "Assets/Mesh/Mesh.h"
#include "Assets/Mesh/MeshMeta.h"
#include "Logcat.h"
#include "Platform/FileSystem/Folder.h"
#include "Platform/FileSystem/Path.h"
#include "Project.h"

#include "SQLiteCpp/Database.h"

void resource::AssetDataBase::Startup()
{
    const auto& proj    = Project::GetCurrentProject();
    const auto  db_path = proj.GetDatabasePath();
    if (!platform::Path::IsExist(db_path))
    {
        platform::Folder::CreateFolder(db_path);
    }
    core::Assert::Require(logcat::Resource, platform::Path::IsFolder(db_path), "DataBasePath in project must be a valid folder path.");
    const auto db_file = platform::Path::Combine(db_path, "AssetDataBase.db");
    db_                = New<SQLite::Database>(db_file.Data(), SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
    core::resource::SQLHelper::InitializeDataBase(*db_);
    CreateAssetTables();
}

void resource::AssetDataBase::Shutdown()
{
    Delete(db_);
    db_ = nullptr;
}

void resource::AssetDataBase::Import(core::StringView path)
{
    // 先查一下是否存在, 存在的话按现有配置重新导入
    auto  query    = core::String::Format("path = '{}'", path);
    auto& registry = core::ObjectManager::GetRegistry();
    if (path.EndsWith(".fbx"))
    {
        auto result = QueryMeta<MeshMeta>(query);
        if (result)
        {
            auto&              meta   = *result;
            core::ObjectHandle handle = meta.GetObjectId();
            auto*              obj    = registry.GetObjectByHandle(handle);
            if (obj != nullptr)
            {
                registry.RemoveObject(obj);
            }
            auto* mesh = New<resource::Mesh>();
            mesh->InternalSetAssetHandle(handle);
            mesh->InternalPerformPersistentObjectLoad();
        }
        else
        {
            MeshMeta new_meta      = {};
            new_meta.path          = path;
            new_meta.object_handle = registry.NextPersistentHandle();
            InsertMeta(new_meta);
            auto* mesh = New<Mesh>();
            mesh->InternalSetAssetHandle(new_meta.object_handle);
            mesh->InternalPerformPersistentObjectLoad();
        }
    }
}

#define CREATE_ASSET_TABLE(asset_type)                   \
    const core::Type* type = core::TypeOf<asset_type>(); \
    tables_[type]          = std::move(core::resource::SQLHelper::CreateTable(*db_, type));

void resource::AssetDataBase::CreateAssetTables()
{
    CREATE_ASSET_TABLE(resource::MeshMeta);
}
