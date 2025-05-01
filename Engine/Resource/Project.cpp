//
// Created by Echo on 2025/3/27.
//

#include "Project.hpp"

#include "Core/FileSystem/File.hpp"
#include "Core/FileSystem/Path.hpp"
#include "Core/Profile.hpp"
#include "Core/Serialization/XMLArchive.hpp"

#include <fstream>

Project::Project(const StringView InPath)
{
    ProfileScope _(__func__);
    String MetaFilePath = Path::Combine(InPath, ".elbowproj");
    try
    {
        std::ifstream MetaFile(*MetaFilePath);
        if (!MetaFile)
        {
            Log(Fatal) << String::Format("打开项目元文件失败: {}", *InPath);
            return;
        }
        XMLInputArchive Archive(MetaFile);
        Archive.Deserialize(*this);
    }
    catch (const std::exception& Ex)
    {
        VLOG_ERROR("项目文件似有损毁, 将重置项目元信息.");
        mName = "New Project";
        mPath = InPath;
        mVersion = "0.0.1";
        mDatabasePath = "Library/AssetDataBase";
        std::ofstream OutputStream(*MetaFilePath);
        XMLOutputArchive Archive(OutputStream);
        Archive.Serialize(*this);
    }
}

struct BeforeMainTrigger_Project
{
    BeforeMainTrigger_Project()
    {
        Evt_OnProjectPathSet.AddBind(&Project::CreateInstance);
    }
};

static inline BeforeMainTrigger_Project beforeMainTrigger_Project;

inline Optional<Project> gLoadedProject;

Project& Project::GetCurrentProject()
{
    return *gLoadedProject;
}

void Project::CreateInstance(const StringView InPath)
{
    if (gLoadedProject)
    {
        Log(Fatal) << "Project不能被加载两次";
    }
    gLoadedProject = Project(InPath);
}
