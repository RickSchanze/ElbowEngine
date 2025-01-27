/**
 * @file AssetDataBase.cpp
 * @author Echo
 * @Date 24-12-4
 * @brief
 */

#include "AssetDataBase.h"

#include "Assets/Material/Material.h"
#include "Assets/Material/MaterialMeta.h"
#include "Assets/Mesh/Mesh.h"
#include "Assets/Mesh/MeshMeta.h"
#include "Assets/Shader/Shader.h"
#include "Assets/Shader/ShaderMeta.h"
#include "Assets/Texture/Texture2D.h"
#include "Assets/Texture/Texture2DMeta.h"
#include "Core/Object/ObjectRegistry.h"
#include "Core/Profiler/ProfileMacro.h"
#include "Core/Serialization/YamlArchive.h"
#include "Logcat.h"
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
AsyncResultHandle<ObjectHandle> InternalImport(StringView query, StringView path, ObjectRegistry &registry) {
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
    auto &[asset] = *(registry.CreateNewObject<T>()->GetValue());
    new_meta.object_handle = std::get<0>(*registry.NextPersistentHandle()->GetValue());
    AssetDataBase::InsertMeta(new_meta);
    asset->InternalSetAssetHandle(new_meta.object_handle);
    return asset->PerformPersistentObjectLoadAsync();
  }
}

AsyncResultHandle<ObjectHandle> AssetDataBase::Import(StringView path) {
  PROFILE_SCOPE_AUTO;
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
  return MakeAsyncResult(0);
}

Object *AssetDataBase::Load(StringView path) {
  PROFILE_SCOPE_AUTO;
  auto handle = LoadAsync(path);
  if (!handle)
    return nullptr;
  handle->Wait();
  auto op = handle->GetValue();
  if (!op) {
    return nullptr;
  }
  auto [obj] = *op;
  return ObjectManager::GetObjectByHandle(obj);
}

template <typename T, typename TMeta> AsyncResultHandle<ObjectHandle> InternalLoadAsync(StringView path) {
  if (const auto meta_op = AssetDataBase::QueryMeta<TMeta>(String::Format("path = '{}'", path)); !meta_op) {
    LOGGER.Warn(logcat::Resource_Load, "资产{}未在资产数据库中找到.", path);
    return NULL_ASYNC_RESULT_HANDLE;
  } else {
    auto &meta = *meta_op;
    ObjectHandle handle = meta.object_handle;
    ObjectRegistry &registry = ObjectManager::GetRegistry();
    Object *obj = registry.GetObjectByHandle(handle);
    if (obj != nullptr) {
      return MakeAsyncResult(handle);
    } else {
      auto [asset] = *ObjectManager::GetRegistry().CreateNewObject<T>()->GetValue();
      asset->InternalSetAssetHandle(handle);
      return asset->PerformPersistentObjectLoadAsync();
    }
  }
}

AsyncResultHandle<ObjectHandle> AssetDataBase::LoadAsync(StringView path) {
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
  if (path.EndsWith(".mat")) {
    // 查看资产数据库是否存在
    auto meta_op = AssetDataBase::QueryMeta<MaterialMeta>(String::Format("path = '{}'", path));
    if (meta_op) {
      auto &meta = *meta_op;
      ObjectHandle handle = meta.object_handle;
      ObjectRegistry &registry = ObjectManager::GetRegistry();
      Object *obj = registry.GetObjectByHandle(handle);
      if (obj != nullptr) {
        return MakeAsyncResult(handle);
      } else {
        auto [asset] = *ObjectManager::GetRegistry().CreateNewObject<Material>()->GetValue();
        asset->InternalSetAssetHandle(handle);
        YamlArchive archive;
        auto content = File::ReadAllText(path);
        if (content) {
          if (archive.Deserialize(*content, asset, TypeOf<Material>())) {
            return asset->PerformPersistentObjectLoadAsync();
          }
        }
        return MakeAsyncResult(0);
      }
    }
  }
  return NULL_ASYNC_RESULT_HANDLE;
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
}

AsyncResultHandle<Object *> AssetDataBase::LoadAsync(ObjectHandle handle, const core::Type *asset_type) {
  if (asset_type == nullptr)
    return MakeAsyncResult<Object *>(nullptr);
  if (ObjectManager::IsObjectExist(handle)) {
    return MakeAsyncResult(ObjectManager::GetObjectByHandle(handle));
  }
  core::String path;
  if (asset_type == TypeOf<Shader>()) {
    path = QueryPath<Shader>(handle);
  }
  if (path.IsEmpty())
    return MakeAsyncResult<Object *>(nullptr);
  auto op_handle = LoadAsync(path)->GetValue();
  if (!op_handle) {
    return MakeAsyncResult<Object *>(nullptr);
  }
  auto [obj] = *op_handle;
  return MakeAsyncResult<Object *>(ObjectManager::GetObjectByHandle(obj));
}

#define CREATE_ASSET_TABLE(asset_type)                                                                                 \
  {                                                                                                                    \
    const core::Type *type = core::TypeOf<asset_type>();                                                               \
    tables_[type] = std::move(resource::SQLHelper::CreateTable(*db_, type));                                           \
  }

void AssetDataBase::CreateAsset(Asset *asset, StringView path) {
  const Type *type = asset->GetType();
  if (type == TypeOf<Texture2D>() || type == TypeOf<Mesh>() || type == TypeOf<Shader>()) {
    LOGGER.Error(logcat::Resource, "{}只支持导入而不支持创建.", type->GetName());
    return;
  }
  if (type == TypeOf<Material>()) {
    auto handle = ObjectManager::GetRegistry().NextPersistentHandle()->GetValue();
    asset->InternalSetAssetHandle(std::get<0>(*handle));
    YamlArchive archive;
    String serialized_str;
    Material &material = *static_cast<Material *>(asset);
    if (!archive.Serialize(material, serialized_str)) {
      LOGGER.Error(logcat::Resource, "无法序列化Material.");
      return;
    }
    platform::File::WriteAllText(path, serialized_str);
    MaterialMeta meta;
    meta.object_handle = handle;
    meta.path = path;
    InsertMeta(meta);
  }
}

void AssetDataBase::CreateAssetTables() {
  CREATE_ASSET_TABLE(::resource::MeshMeta);
  CREATE_ASSET_TABLE(::resource::ShaderMeta);
  CREATE_ASSET_TABLE(::resource::Texture2DMeta);
  CREATE_ASSET_TABLE(::resource::MaterialMeta);
}
