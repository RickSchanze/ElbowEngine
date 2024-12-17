/**
 * @file AssetDataBase.cpp
 * @author Echo 
 * @Date 24-12-4
 * @brief 
 */

#include "AssetDataBase.h"

#include "Logcat.h"
#include "Meta/MeshMeta.h"
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
    db_ = New<SQLite::Database>(db_file.Data(), SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
    core::resource::SQLHelper::InitializeDataBase(*db_);
    CreateAssetTables();
}

void resource::AssetDataBase::Shutdown()
{
    Delete(db_);
    db_ = nullptr;
}

#define CREATE_ASSET_TABLE(asset_type)                   \
    const core::Type* type = core::TypeOf<asset_type>(); \
    tables_[type]          = core::resource::SQLHelper::CreateTable(*db_, type);

void resource::AssetDataBase::CreateAssetTables()
{
    CREATE_ASSET_TABLE(core::resource::MeshMeta)
}
