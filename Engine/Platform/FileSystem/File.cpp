/**
 * @file File.cpp
 * @author Echo
 * @Date 24-11-9
 * @brief
 */

#include "File.h"

#include "Core/CoreEvents.h"
#include "Core/Log/Logger.h"
#include "Folder.h"
#include "Path.h"
#include "Platform/PlatformLogcat.h"
#include <filesystem>
#include <fstream>

using namespace core;
namespace fs = std::filesystem;

bool platform::File::Create_File(StringView path, FileCreateMode mode, bool create_folder, bool overwrite) {
  if (Path::IsExist(path)) {
    if (!overwrite) {
      LOGGER.Error(logcat::Platform_FileSystem, "File already exist: {}", path);
      return false;
    }
  }
  auto parent_path = Path::GetParent(path);
  if (!Path::IsExist(parent_path)) {
    if (!create_folder) {
      LOGGER.Error(logcat::Platform_FileSystem, "Parent folder not exist: {}", parent_path);
      return false;
    }
    if (!Folder::CreateFolder(parent_path)) {
      LOGGER.Error(logcat::Platform_FileSystem, "Failed to create parent folder: {}", parent_path);
      return false;
    }
  }
  if (mode == FileCreateMode::Text) {
    if (const std::ofstream file(path.Data(), std::ios::binary); !file) {
      return false;
    }
  } else if (mode == FileCreateMode::Binary) {
    if (const std::ofstream file(path.Data(), std::ios::binary); !file) {
      return false;
    }
  }
  return true;
}

String platform::File::GetFolder() const { return std::filesystem::path(path_.ToStdString()).parent_path().string(); }

String platform::File::GetFileName() const { return std::filesystem::path(path_.ToStdString()).filename().string(); }

String platform::File::GetFileNameWithoutExtension() const {
  return std::filesystem::path(path_.ToStdString()).stem().string();
}

String platform::File::GetAbsolutePath() const {
  // 看看是不是已经是绝对路径了
  fs::path p = path_.ToStdString();
  if (p.is_absolute())
    return path_;
  return Path::GetProjectPath() + "/" + path_;
}

String platform::File::GetRelativePath() const { return path_; }

bool platform::File::IsExist() const {
  const auto s = GetAbsolutePath();

  return Path::IsExist(s);
}

bool platform::File::IsExist(StringView path) { return File(path).IsExist(); }

bool platform::File::TryReadAllText(String &out) const {
  if (!IsExist()) {
    LOGGER.Error(logcat::Platform_FileSystem, "File not exist: {}", path_);
    return false;
  }
  std::ifstream file(path_.Data(), std::ios::in);
  if (!file.is_open()) {
    LOGGER.Error(logcat::Platform_FileSystem, "Failed to open file: {}", path_);
    return false;
  }
  out.Clear();
  out = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  return true;
}

Optional<String> platform::File::ReadAllText() const {
  if (String text; TryReadAllText(text)) {
    return MakeOptional(text);
  }
  return {};
}

Optional<String> platform::File::ReadAllText(const String &path) {
  const File file = path;
  return file.ReadAllText();
}

bool platform::File::WriteAllText(StringView text) const {
  if (Create()) {
    std::ofstream file(path_.Data(), std::ios::out);
    if (!file.is_open()) {
      LOGGER.Error(logcat::Platform_FileSystem, "Failed to open file: {}", path_);
      return false;
    }
    file << text;
    return true;
  } else {
    LOGGER.Error(logcat::Platform_FileSystem, "Failed to write file: {}", path_);
    return false;
  }
}

bool platform::File::WriteAllText(const String &path, StringView text) {
  const File file = path;
  return file.WriteAllText(text);
}

bool platform::File::Create(FileCreateMode mode, bool create_folder, bool overwrite) const {
  return Create_File(path_, mode, create_folder, overwrite);
}

namespace {
struct StaticInit {
  static Optional<String> ReadText(const String &path) {
    const platform::File file(path);
    return file.ReadAllText();
  }

  static bool WriteText(const String &path, const String &text) {
    const platform::File f = path;
    return f.WriteAllText(text);
  }

  static void BindOther(StringView path) {
    Event_OnRequireReadFileText.Bind(&StaticInit::ReadText);
    Event_OnWriteFileText.Bind(&StaticInit::WriteText);
  }

  StaticInit() { Event_OnProjectPathSet.AddBind(&StaticInit::BindOther); }
};
[[maybe_unused]] static const StaticInit static_init;
} // namespace
