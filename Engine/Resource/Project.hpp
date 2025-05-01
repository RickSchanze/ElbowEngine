//
// Created by Echo on 2025/3/23.
//
#pragma once
#include "Core/Core.hpp"

#include GEN_HEADER("Project.generated.hpp")

/**
 * Project阐述了一个项目的基本信息
 * 例如 项目名称 项目路径 项目版本号 项目数据库位置等
 */
struct ESTRUCT() Project
{
    GENERATED_BODY(Project)

    static Project& GetCurrentProject();
    static void CreateInstance(StringView InPath);

    StringView GetProjectName() const
    {
        return mName;
    }
    StringView GetProjectPath() const
    {
        return mPath;
    }
    StringView GetVersion() const
    {
        return mVersion;
    }
    StringView GetDatabasePath() const
    {
        return mDatabasePath;
    }

protected:
    Project() = default;

    /**
     * 传入项目文件夹
     * @param InPath
     */
    explicit Project(StringView InPath);

private:
    EFIELD()
    String mName{"New Project"};

    EFIELD()
    String mPath{};

    EFIELD()
    String mVersion{"0.0.1"};

    /// 这个是资产数据库的路径
    EFIELD()
    String mDatabasePath{"Library/AssetDataBase"};
};
