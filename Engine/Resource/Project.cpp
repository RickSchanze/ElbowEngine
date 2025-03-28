//
// Created by Echo on 2025/3/27.
//

#include "Project.hpp"

#include "Core/FileSystem/File.hpp"
#include "Core/FileSystem/Path.hpp"
#include "Core/Profile.hpp"
#include "Core/Serialization/YamlArchive.hpp"

Project::Project(const StringView path) {
    ProfileScope _(__func__);
    auto meta_text = File::ReadAllText(".elbowengine");
    Assert(meta_text, "Failed to read project meta file.");
    if (const auto &meta = *meta_text; meta.IsEmpty()) {
        YamlArchive ar;
        name_ = "New Project";
        path_ = path;
        version_ = "0.0.1";
        database_path_ = "Library/AssetDataBase";
        bool success = true;
        String temp;
        success &= ar.Serialize(*this, temp);
        success &= File::WriteAllText(".elbowengine", temp);
        Assert(success, "Failed to create project {}", name_);
        return;
    }
    YamlArchive ar;
    ar.Deserialize(*meta_text, this, TypeOf<Project>());
}


struct BeforeMainTrigger_Project {
    BeforeMainTrigger_Project() {
        ReflManager::GetByRef().Register<Project>();
        Evt_OnProjectPathSet.AddBind(&Project::CreateInstance);
    }
};

static inline BeforeMainTrigger_Project beforeMainTrigger_Project;
Type *Project::ConstructType() {
    return Type::Create<Project>("Project") | refl_helper::AddField("name", &Project::name_) | refl_helper::AddField("path", &Project::path_) |
           refl_helper::AddField("version", &Project::version_) | refl_helper::AddField("database_path", &Project::database_path_);
}

inline Optional<Project> loaded_project;

Project &Project::GetCurrentProject() { return *loaded_project; }

void Project::CreateInstance(StringView path) {
    if (loaded_project) {
        Log(Fatal) << "Project不能被加载两次";
    }
    loaded_project = Project(path);
}
