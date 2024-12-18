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
    return *loaded_project;
}

void resource::Project::CreateInstance(core::StringView path)
{
    if (loaded_project.HasValue())
    {
        LOGGER.Warn(logcat::Resource_Project, "Project already loaded. This loading will be ignored.");
        return;
    }
    loaded_project = Project(path);
}

resource::Project::Project(core::StringView path)
{
    auto meta_text = platform::File::ReadAllText(".elbowengine");
    core::Assert::Require(logcat::Engine, meta_text.HasValue(), "Failed to read project meta file.");
    if (const auto& meta = *meta_text; meta.IsEmpty())
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
        core::Assert::Require(logcat::Resource_Project, success, "Failed to create project {}", name_);
        return;
    }
    core::YamlArchive ar;
    ar.Deserialize(*meta_text, this, core::TypeOf<Project>());
}

REGISTER_EVENT_HANDLER_BEFORE_MAIN()
{
    Event_OnProjectPathSet.AddBind(&resource::Project::CreateInstance);
}
