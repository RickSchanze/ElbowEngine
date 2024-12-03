/**
 * @file Project.cpp
 * @author Echo 
 * @Date 24-12-1
 * @brief 
 */

#include "Project.h"

#include "Core/CoreEvents.h"
#include "Core/Event/Event.h"
#include "Platform/FileSystem/Path.h"

#include "Core/Serialization/YamlArchive.h"
#include GEN_HEADER("Resource.Project.generated.h")

GENERATED_SOURCE()

resource::Project& resource::Project::GetInstance()
{
    static Project p;
    return p;
}

void resource::Project::CreateInstance(core::StringView path) {}

resource::Project::Project(core::StringView path)
{
    auto proj_meta_path = platform::Path::Combine(path, ".elbowengine");
    auto meta_text      = platform::File::ReadAllText(proj_meta_path);
    Assert(logcat::Engine, meta_text.has_value(), "Failed to read project meta file: {}", proj_meta_path);
    auto& meta = meta_text.value();
    if (meta.IsEmpty())
    {
        name_          = "New Project";
        path_          = path;
        version_       = "0.0.1";
        database_path_ = "Library/AssetDataBase";
        return;
    }
}

REGISTER_EVENT_HANDLER_BEFORE_MAIN()
{
    Event_OnProjectPathSet.AddBind(&resource::Project::CreateInstance);
}
