/**
 * @file Project.cpp
 * @author Echo 
 * @Date 24-12-1
 * @brief 
 */

#include "Project.h"
#include "Core/CoreEvents.h"
#include "Core/Event/Event.h"
#include "Core/Serialization/YamlArchive.h"
#include "Logcat.h"
#include "Platform/FileSystem/Path.h"

#include GEN_HEADER("Resource.Project.generated.h")

GENERATED_SOURCE()

core::Optional<resource::Project> loaded_project;

resource::Project& resource::Project::GetCurrentProject()
{
    return loaded_project.value();
}

void resource::Project::CreateInstance(core::StringView path)
{
    if (loaded_project.has_value())
    {
        LOGGER.Warn(logcat::Resource_Project, "Project already loaded. This loading will be ignored.");
        return;
    }
    loaded_project = Project(path);
}

resource::Project::Project(core::StringView path)
{
    auto meta_text = platform::File::ReadAllText(".elbowengine");
    Assert(logcat::Engine, meta_text.has_value(), "Failed to read project meta file.");
    if (const auto& meta = meta_text.value(); meta.IsEmpty())
    {
        core::YamlArchive ar;
        name_                = "New Project";
        path_                = path;
        version_             = "0.0.1";
        database_path_       = "Library/AssetDataBase";
        bool         success = true;
        core::String temp;
        success &= ar.Serialize(*this, temp);
        success &= platform::File::WriteAllText(".elbowengine", temp);
        Assert(logcat::Resource_Project, success, "Failed to create project {}", name_);
        return;
    }
}

REGISTER_EVENT_HANDLER_BEFORE_MAIN()
{
    Event_OnProjectPathSet.AddBind(&resource::Project::CreateInstance);
}
