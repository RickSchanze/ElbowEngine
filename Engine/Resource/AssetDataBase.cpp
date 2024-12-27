/**
 * @file AssetDataBase.cpp
 * @author Echo 
 * @Date 24-12-4
 * @brief 
 */

#include "AssetDataBase.h"

#include "Assets/Mesh/Mesh.h"
#include "Assets/Mesh/MeshMeta.h"
#include "Assets/Shader/Shader.h"
#include "Assets/Shader/ShaderMeta.h"
#include "Logcat.h"
#include "Platform/FileSystem/Folder.h"
#include "Platform/FileSystem/Path.h"
#include "Project.h"

#include "SQLiteCpp/Database.h"

using namespace resource;
using namespace core;
using namespace core::exec;

void AssetDataBase::Startup()
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

void AssetDataBase::Shutdown()
{
    Delete(db_);
    db_ = nullptr;
}

template <typename T, typename TMeta>
AsyncResultHandle InternalImport(StringView query, StringView path, ObjectRegistry& registry)
{
    auto result = AssetDataBase::QueryMeta<TMeta>(query);
    if (result)
    {
        auto&        meta   = *result;
        ObjectHandle handle = meta.object_handle;
        auto*        obj    = registry.GetObjectByHandle(handle);
        if (obj != nullptr)
        {
            registry.RemoveObject(obj);
        }
        auto* asset = New<T>();
        asset->InternalSetAssetHandle(handle);
        return asset->InternalPerformPersistentObjectLoad();
    }
    else
    {
        TMeta new_meta         = {};
        new_meta.path          = path;
        new_meta.object_handle = registry.NextPersistentHandle();
        AssetDataBase::InsertMeta(new_meta);
        auto* asset = New<T>();
        asset->InternalSetAssetHandle(new_meta.object_handle);
        return asset->InternalPerformPersistentObjectLoad();
    }
}

AsyncResultHandle AssetDataBase::Import(StringView path)
{
    // 先查一下是否存在, 存在的话按现有配置重新导入
    auto  query    = String::Format("path = '{}'", path);
    auto& registry = ObjectManager::GetRegistry();
    if (path.EndsWith(".fbx"))
    {
        return InternalImport<Mesh, MeshMeta>(query, path, registry);
    }
    if (path.EndsWith(".slang"))
    {
        return InternalImport<Shader, ShaderMeta>(query, path, registry);
    }
    return NULL_ASYNC_RESULT_HANDLE;
}

#define CREATE_ASSET_TABLE(asset_type)                                                          \
    {                                                                                           \
        const core::Type* type = core::TypeOf<asset_type>();                                    \
        tables_[type]          = std::move(core::resource::SQLHelper::CreateTable(*db_, type)); \
    }

void AssetDataBase::CreateAssetTables()
{
    CREATE_ASSET_TABLE(::resource::MeshMeta);
    CREATE_ASSET_TABLE(::resource::ShaderMeta);
}
