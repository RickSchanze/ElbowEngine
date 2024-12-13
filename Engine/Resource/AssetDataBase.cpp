/**
 * @file AssetDataBase.cpp
 * @author Echo 
 * @Date 24-12-4
 * @brief 
 */

#include "AssetDataBase.h"

#include "Logcat.h"
#include "Platform/FileSystem/Folder.h"
#include "Platform/FileSystem/Path.h"
#include "Project.h"

static void CreateOrOpenDataBase(resource::AssetDataBase::DataBaseArray& data_bases) {}

void resource::AssetDataBase::Startup()
{
    auto& proj    = Project::GetCurrentProject();
    auto  db_path = proj.GetDatabasePath();
    if (!platform::Path::IsExist(db_path))
    {
        platform::Folder::CreateFolder(db_path);
    }
    core::Assert::Require(logcat::Resource, platform::Path::IsFolder(db_path), "DataBasePath in project must be a valid folder path.");
    CreateOrOpenDataBase(databases_);
}
