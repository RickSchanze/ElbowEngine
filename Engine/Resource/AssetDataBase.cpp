/**
 * @file AssetDataBase.cpp
 * @author Echo
 * @Date 24-12-4
 * @brief
 */

#include "AssetDataBase.h"

#include "Assets/Font/Font.h"
#include "Assets/Font/FontMeta.h"
#include "Assets/Material/Material.h"
#include "Assets/Material/MaterialMeta.h"
#include "Assets/Mesh/Mesh.h"
#include "Assets/Mesh/MeshMeta.h"
#include "Assets/Shader/Shader.h"
#include "Assets/Shader/ShaderMeta.h"
#include "Assets/Texture/Texture2D.h"
#include "Assets/Texture/Texture2DMeta.h"
#include "Core/Base/Ranges.h"
#include "Core/Object/ObjectRegistry.h"
#include "Core/Profiler/ProfileMacro.h"
#include "Core/Serialization/YamlArchive.h"
#include "Logcat.h"
#include "Platform/FileSystem/File.h"
#include "Platform/FileSystem/Folder.h"
#include "Platform/FileSystem/Path.h"
#include "Project.h"

#include "SQLiteCpp/Database.h"

using namespace resource;
using namespace core;
using namespace core::exec;
using namespace platform;

void AssetDataBase::Startup() {
  const auto &proj = Project::GetCurrentProject();
  const auto db_path = proj.GetDatabasePath();
  if (!platform::Path::IsExist(db_path)) {
    platform::Folder::CreateFolder(db_path);
  }
  Assert::Require(logcat::Resource, platform::Path::IsFolder(db_path),
                  "DataBasePath in project must be a valid folder path.");
  const auto db_file = platform::Path::Combine(db_path, "AssetDataBase.db");
  db_ = New<SQLite::Database>(db_file.Data(), SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
  SQLHelper::InitializeDataBase(*db_);
  CreateAssetTables();
}

void AssetDataBase::Shutdown() {
  Delete(db_);
  db_ = nullptr;
}

template <typename T, typename TMeta>
ExecFuture<ObjectHandle> InternalImport(StringView query, StringView path, ObjectRegistry &registry) {
  auto result = AssetDataBase::QueryMeta<TMeta>(query);
  if (result) {
    auto &meta = *result;
    ObjectHandle handle = meta.object_handle;
    auto *obj = registry.GetObjectByHandle(handle);
    if (obj != nullptr) {
      registry.RemoveObject(obj);
    }
    auto *asset = New<T>();
    asset->InternalSetAssetHandle(handle);
    return asset->PerformPersistentObjectLoadAsync();
  } else {
    TMeta new_meta = {};
    new_meta.path = path;
    auto *asset = ObjectManager::CreateNewObjectAsync<T>().Get();
    new_meta.object_handle = registry.NextPersistentHandle().Get();
    AssetDataBase::InsertMeta(new_meta);
    asset->InternalSetAssetHandle(new_meta.object_handle);
    return asset->PerformPersistentObjectLoadAsync();
  }
}

exec::ExecFuture<ObjectHandle> AssetDataBase::Import(StringView path) {
  PROFILE_SCOPE_AUTO;
  Assert::Require("Resource.Import", IsMainThread(), "Import只能在主线程调用");
  // 先查一下是否存在, 存在的话按现有配置重新导入
  auto query = String::Format("path = '{}'", path);
  auto &registry = ObjectManager::GetRegistry();
  if (path.EndsWith(".fbx")) {
    return InternalImport<Mesh, MeshMeta>(query, path, registry);
  }
  if (path.EndsWith(".slang")) {
    return InternalImport<Shader, ShaderMeta>(query, path, registry);
  }
  if (path.EndsWith(".png")) {
    return InternalImport<Texture2D, Texture2DMeta>(query, path, registry);
  }
  if (path.EndsWith(".ttf")) {
    return InternalImport<Font, FontMeta>(query, path, registry);
  }
  return MakeExecFuture(0);
}

Object *AssetDataBase::Load(StringView path) {
  PROFILE_SCOPE_AUTO;
  const ObjectHandle obj_handle = LoadAsync(path).Get();
  return ObjectManager::GetObjectByHandle(obj_handle);
}

template <typename T, typename TMeta> ExecFuture<ObjectHandle> InternalLoadAsync(StringView path) {
  if (const auto meta_op = AssetDataBase::QueryMeta<TMeta>(String::Format("path = '{}'", path)); !meta_op) {
    LOGGER.Warn(logcat::Resource_Load, "资产{}未在资产数据库中找到.", path);
    return MakeExecFuture(0);
  } else {
    auto &meta = *meta_op;
    ObjectHandle handle = meta.object_handle;
    ObjectRegistry &registry = ObjectManager::GetRegistry();
    Object *obj = registry.GetObjectByHandle(handle);
    if (obj != nullptr) {
      return MakeExecFuture(handle);
    } else {
      T *asset = ObjectManager::CreateNewObjectAsync<T>().Get();
      asset->InternalSetAssetHandle(handle);
      return asset->PerformPersistentObjectLoadAsync();
    }
  }
}

ExecFuture<ObjectHandle> AssetDataBase::LoadAsync(StringView path) {
  PROFILE_SCOPE_AUTO;
  if (path.EndsWith(".slang")) {
    return InternalLoadAsync<Shader, ShaderMeta>(path);
  }
  if (path.EndsWith(".fbx")) {
    return InternalLoadAsync<Mesh, MeshMeta>(path);
  }
  if (path.EndsWith(".png")) {
    return InternalLoadAsync<Texture2D, Texture2DMeta>(path);
  }
  if (path.EndsWith(".ttf")) {
    return InternalLoadAsync<Font, FontMeta>(path);
  }
  if (path.EndsWith(".mat")) {
    // 查看资产数据库是否存在
    if (Optional<MaterialMeta> meta_op = QueryMeta<MaterialMeta>(String::Format("path = '{}'", path))) {
      const MaterialMeta &meta = *meta_op;
      ObjectHandle database_handle = meta.object_handle;
      ObjectRegistry &registry = ObjectManager::GetRegistry();
      if (const Object *obj = registry.GetObjectByHandle(database_handle); obj != nullptr) {
        return MakeExecFuture(database_handle);
      } else {
        // 创建一个instanced asset作为临时载入, 此时它被注册入registry
        Material *asset = ObjectManager::CreateNewObjectAsync<Material>().Get();
        YamlArchive archive;
        auto content = File::ReadAllText(path);
        auto old_instanced_handle = asset->GetHandle();
        if (content) {
          // 序列化一个object 如果成功那么这个persistent handle也会被入册到registry
          // 此时registry有两个一样的对象, 一是之前作为临时instanced object注册的, 在一个就是序列化时注册
          if (archive.Deserialize(*content, asset, TypeOf<Material>())) {
            auto file_handle = asset->GetHandle();
            Assert::Require(logcat::Resource_Load, file_handle == database_handle, "file_handle != database_handle");
            // 校验通过则将临时注册的取消注册
            ObjectManager::GetRegistry().UnregisterHandle(old_instanced_handle);
            auto rtn = asset->PerformPersistentObjectLoadAsync();
            return rtn;
          }
        }
        return MakeExecFuture(0);
      }
    }
  }
  return MakeExecFuture(0);
}

template <typename T> static core::String QueryPath(core::ObjectHandle handle) {
  if constexpr (std::is_same_v<T, Shader>) {
    auto meta_op = AssetDataBase::QueryMeta<ShaderMeta>(handle);
    if (!meta_op) {
      LOGGER.Warn(logcat::Resource_Load, "资产{}未在资产数据库中找到.", handle);
      return "";
    }
    auto &meta = *meta_op;
    return meta.path;
  }
  return "";
}

ExecFuture<Object *>  AssetDataBase::LoadAsync(ObjectHandle handle, const core::Type *asset_type) {
  if (asset_type == nullptr)
    return MakeExecFuture<Object *>(nullptr);
  if (ObjectManager::IsObjectExist(handle)) {
    return MakeExecFuture(ObjectManager::GetObjectByHandle(handle));
  }
  core::String path;
  if (asset_type == TypeOf<Shader>()) {
    path = QueryPath<Shader>(handle);
  }
  if (path.IsEmpty())
    return MakeExecFuture<Object *>(nullptr);
  auto obj_handle = LoadAsync(path).Get();
}

#define CREATE_ASSET_TABLE(asset_type)                                                                                 \
  {                                                                                                                    \
    const core::Type *type = core::TypeOf<asset_type>();                                                               \
    tables_[type] = std::move(resource::SQLHelper::CreateTable(*db_, type));                                           \
  }

bool AssetDataBase::CreateAsset(Asset *asset, StringView path) {
  if (asset == nullptr) {
    LOGGER.Error("Resource", "CreateAsset: 传入asset为空");
    return false;
  }
  const Type *type = asset->GetType();
  if (type == TypeOf<Mesh>() || type == TypeOf<Shader>()) {
    LOGGER.Error(logcat::Resource, "{}只支持导入而不支持创建.", type->GetName());
    return false;
  }
  if (type == TypeOf<Material>()) {
    if (auto exist_meta = QueryMeta<MaterialMeta>(String::Format("path = '{}'", path))) {
      LOGGER.Error("Resource", "CreateAsset: {}已存在", path);
      return false;
    }
    ObjectHandle handle = ObjectManager::GetRegistry().NextPersistentHandle().Get();
    asset->InternalSetAssetHandle(handle);
    YamlArchive archive;
    String serialized_str;
    Material &material = *static_cast<Material *>(asset);
    if (!archive.Serialize(material, serialized_str)) {
      LOGGER.Error(logcat::Resource, "无法序列化Material.");
      return false;
    }
    File::WriteAllText(path, serialized_str);
    MaterialMeta meta;
    meta.object_handle = handle;
    meta.path = path;
    InsertMeta(meta);
    return true;
  }
  if (type == TypeOf<Texture2D>()) {
    if (auto exist_meta = QueryMeta<Texture2DMeta>(String::Format("path = '{}'", path))) {
      LOGGER.Error("Resource", "CreateAsset: {}已存在", path);
      return false;
    }
    ObjectHandle handle = ObjectManager::GetRegistry().NextPersistentHandle().Get();
    asset->InternalSetAssetHandle(handle);
    auto tex = static_cast<Texture2D *>(asset);
    Assert::Require("Resource", Equals(tex->GetAssetPath(), path), "创建纹理资源失败: 路径不匹配");
    Texture2DMeta meta;
    meta.object_handle = handle;
    meta.path = path;
    meta.dynamic = false;
    meta.height = tex->GetHeight();
    meta.width = tex->GetWidth();
    meta.format = tex->GetFormat();
    meta.sprites_string = tex->GetSpriteRangeString();
    InsertMeta(meta);
    return true;
  }
  return false;
}

void AssetDataBase::CreateAssetTables() {
  CREATE_ASSET_TABLE(::resource::MeshMeta);
  CREATE_ASSET_TABLE(::resource::ShaderMeta);
  CREATE_ASSET_TABLE(::resource::Texture2DMeta);
  CREATE_ASSET_TABLE(::resource::MaterialMeta);
  CREATE_ASSET_TABLE(::resource::FontMeta);
}
