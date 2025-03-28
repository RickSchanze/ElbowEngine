//
// Created by Echo on 2025/3/23.
//
#pragma once
#include "Core/Core.hpp"

/**
 * Project阐述了一个项目的基本信息
 * 例如 项目名称 项目路径 项目版本号 项目数据库位置等
 */
REFLECTED()
class Project {
  REFLECTED_SINGLE_CLASS(Project)
public:
  static Project &GetCurrentProject();
  static void CreateInstance(StringView path);

  [[nodiscard]] StringView GetProjectName() const { return name_; }
  [[nodiscard]] StringView GetProjectPath() const { return path_; }
  [[nodiscard]] StringView GetVersion() const { return version_; }
  [[nodiscard]] StringView GetDatabasePath() const { return database_path_; }

protected:
  Project() = default;
  explicit Project(StringView path);

private:
  REFLECTED()
  String name_{"New Project"};

  REFLECTED()
  String path_{};

  REFLECTED()
  String version_{"0.0.1"};

  /// 这个是资产数据库的路径
  REFLECTED()
  String database_path_{"Library/AssetDataBase"};
};
