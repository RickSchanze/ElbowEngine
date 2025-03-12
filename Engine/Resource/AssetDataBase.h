/**
 * @file AssetDataBase.h
 * @author Echo
 * @Date 24-12-4
 * @brief
 */

#pragma once
#include "Assets/Asset.h"
#include "Core/Async/Execution/ExecFuture.h"
#include "Core/Object/ObjectRegistry.h"
#include "Core/Singleton/MManager.h"
#include "Func/Camera/ACameraHolder.h"
#include "SQLHelper.h"

namespace core {
class PersistentObject;
}
namespace resource {
class Texture2DMeta;
class Material;
} // namespace resource
namespace SQLite {
class Database;
}

namespace resource {

class AssetDataBase : public core::Manager<AssetDataBase> {
public:
  [[nodiscard]] core::ManagerLevel GetLevel() const override { return core::ManagerLevel::L8; }
  [[nodiscard]] core::StringView GetName() const override { return "AssetDataBase"; }

  /**
   * 打开/创建资产数据库
   */
  void Startup() override;

  void Shutdown() override;

  /**
   * 导入资产
   * @param path
   * @return 异步handle
   */
  static core::exec::ExecFuture<core::ObjectHandle> Import(core::StringView path);

  /**
   * 加载一个资产
   * @param path
   * @return
   */
  static core::Object *Load(core::StringView path);

  template <typename T>
    requires std::derived_from<T, core::Object>
  static T *Load(core::StringView path) {
    return static_cast<T *>(Load(path));
  }

  static core::exec::ExecFuture<core::ObjectHandle> LoadAsync(core::StringView path);

  static core::exec::ExecFuture<core::ObjectHandle> LoadAsync(core::ObjectHandle handle, const core::Type *asset_type);

  template <typename T> static core::Future<core::ObjectHandle> LoadAsync(core::ObjectPtr<T> obj) {
    return LoadAsync(obj.GetHandle(), core::TypeOf<T>());
  }

  template <typename T> static core::Object *Load(core::ObjectPtr<T> obj) {
    auto res = LoadAsync(obj.GetHandle(), core::TypeOf<T>()).Get();
    return core::ObjectManager::GetObjectByHandle(res);
  }

  template <typename T> static core::Optional<T> QueryMeta(core::ObjectHandle handle);

  template <typename T> static core::Optional<T> QueryMeta(core::StringView where);

  template <typename T> static void InsertMeta(const T &meta);

  /**
   * 创建一个资产并保存
   * @param asset
   * @param path
   */
  static bool CreateAsset(Asset *asset, core::StringView path);

protected:
  void CreateAssetTables();

  SQLite::Database *db_ = nullptr;
  core::HashMap<const core::Type *, core::UniquePtr<SQLTable>> tables_;
  // TODO: 资产加载管理分离至ResourceManager
  // TODO: 并发HashMap
  std::mutex loading_assets_mutex_;
  DECLARE_TRACEABLE_MUTEX(std::mutex, database_query_mutex_, "Mutex to protect database query");
};

template <typename T> core::Optional<T> AssetDataBase::QueryMeta(core::ObjectHandle handle) {
  core::String query = core::String::Format("object_handle = {}", handle);
  return QueryMeta<T>(query);
}

template <typename T> core::Optional<T> AssetDataBase::QueryMeta(core::StringView where) {
  // 如果不加锁访问数据库,就会崩溃
  std::lock_guard query_meta_lock(Get()->database_query_mutex_);
  auto &self = GetByRef();
  const core::Type *meta_type = core::TypeOf<T>();
  if (!self.tables_.contains(meta_type))
    return {};
  auto &table = self.tables_[meta_type];
  auto result = table->Query(meta_type, where);
  if (result.empty())
    return {};
  const core::SharedAny &meta = result[0];
  return meta.AsAny().AsCopy<T>();
}

template <typename T> void AssetDataBase::InsertMeta(const T &meta) {
  const core::Type *meta_type = core::TypeOf<T>();
  if (!Get()->tables_.contains(meta_type))
    throw core::ArgumentException(NAMEOF(meta), "没有注册表类型");
  const auto &table = Get()->tables_[meta_type];
  table->Insert(meta);
}

} // namespace resource
