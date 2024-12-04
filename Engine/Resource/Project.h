/**
 * @file Project.h
 * @author Echo 
 * @Date 24-12-1
 * @brief 
 */

#pragma once
#include "Core/Core.h"

#include GEN_HEADER("Resource.Project.generated.h")

namespace resource
{
/**
 * Project阐述了一个项目的基本信息
 * 例如 项目名称 项目路径 项目版本号 项目数据库位置等
 */
class CLASS() Project final : public core::ITypeGetter
{
    GENERATED_CLASS(Project)
public:
    static Project& GetCurrentProject();
    static void     CreateInstance(core::StringView path);

    [[nodiscard]] core::StringView GetProjectName() const { return name_; }
    [[nodiscard]] core::StringView GetProjectPath() const { return path_; }
    [[nodiscard]] core::StringView GetVersion() const { return version_; }
    [[nodiscard]] core::StringView GetDatabasePath() const { return database_path_; }

protected:
    Project() = default;
    explicit Project(core::StringView path);

private:
    PROPERTY()
    core::String name_{};

    PROPERTY()
    core::String path_{};

    PROPERTY()
    core::String version_{};

    /// 这个是资产数据库的路径
    PROPERTY()
    core::String database_path_{};
};
}
