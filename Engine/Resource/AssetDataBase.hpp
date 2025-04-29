//
// Created by Echo on 2025/3/23.
//
#pragma once
#include "Assets/Texture/Texture2DMeta.hpp"
#include "Core/Async/Exec/ExecFuture.hpp"
#include "Core/Manager/MManager.hpp"
#include "Core/Misc/UniquePtr.hpp"
#include "Core/Object/Object.hpp"
#include "Core/Object/ObjectPtr.hpp"
#include "SQLHelper.hpp"

class Asset;
namespace SQLite {
    class Database;
}

namespace internal {
    Object *GetObjectByHandle(ObjectHandle handle);
}

class AssetDataBase : public Manager<AssetDataBase> {
public:
    virtual Float GetLevel() const override { return 13; }
    virtual StringView GetName() const override { return "AssetDataBase"; }

    /**
     * 打开/创建资产数据库
     */
    virtual void Startup() override;

    virtual void Shutdown() override;

    /**
     * 导入资产
     * @param path
     * @return 异步handle
     */
    static exec::ExecFuture<ObjectHandle> Import(StringView path);

    /**
     * 加载一个资产
     * @param path
     * @return
     */
    static Object *LoadFromPath(StringView path);

    template<typename T>
        requires IsBaseOf<Object, T>
    static T *LoadFromPath(StringView path) {
        return static_cast<T *>(LoadFromPath(path));
    }

    static exec::ExecFuture<ObjectHandle> LoadFromPathAsync(StringView path);

    static exec::ExecFuture<ObjectHandle> LoadAsync(ObjectHandle handle, const Type *asset_type);

    static exec::ExecFuture<ObjectHandle> LoadOrImportAsync(StringView path);
    static Object *LoadOrImport(StringView path);

    template<typename T> requires IsBaseOf<Object, T>
    static T *LoadOrImportT(StringView path) {
        return static_cast<T *>(LoadOrImport(path));
    }

    template<typename T>
    static Future<ObjectHandle> LoadAsync(ObjectPtr<T> obj) {
        return LoadAsync(obj.GetHandle(), TypeOf<T>());
    }

    template<typename T>
    static Object *Load(ObjectPtr<T> obj) {
        auto res = LoadAsync(obj.GetHandle(), TypeOf<T>()).Get();
        return internal::GetObjectByHandle(res);
    }

    template<typename T>
    static Optional<T> QueryMeta(ObjectHandle handle);

    template<typename T>
    static Optional<T> QueryMeta(StringView where);

    template<typename T>
    static void InsertMeta(const T &meta);

    template<typename T>
    static void UpdateMeta(const T &meta);

    /**
     * 创建一个资产并保存
     * @param asset
     * @param path
     */
    static bool CreateAsset(Asset *asset, StringView path);

protected:
    void CreateAssetTables();

    SQLite::Database *db_ = nullptr;
    Map<const Type *, UniquePtr<SQLTable>> tables_;
    // TODO: 资产加载管理分离至ResourceManager
    // TODO: 并发HashMap
    Mutex loading_assets_mutex_;
    Mutex database_query_mutex_;
};

template<typename T>
Optional<T> AssetDataBase::QueryMeta(ObjectHandle handle) {
    String query = String::Format("ObjectHandle = {}", handle);
    return QueryMeta<T>(query);
}

template<typename T>
Optional<T> AssetDataBase::QueryMeta(StringView where) {
    // 如果不加锁访问数据库,就会崩溃
    AutoLock query_meta_lock(Get()->database_query_mutex_);
    auto &self = GetByRef();
    const Type *meta_type = TypeOf<T>();
    if (!self.tables_.Contains(meta_type))
        return {};
    auto &table = self.tables_[meta_type];
    auto result = table->Query(meta_type, where);
    if (result.Empty())
        return {};
    const SharedAny &meta = result[0];
    return meta.AsAny().AsCopy<T>();
}

template<typename T>
void AssetDataBase::InsertMeta(const T &meta) {
    const Type *meta_type = TypeOf<T>();
    if (!Get()->tables_.Contains(meta_type))
        Log(Error) << "没有注册表类型";
    const auto &table = Get()->tables_[meta_type];
    table->Insert(meta);
}

template<typename T>
void AssetDataBase::UpdateMeta(const T &meta) {
    if constexpr (SameAs<T, Texture2DMeta>) {
        auto &self = GetByRef();
        const Type *meta_type = TypeOf<T>();
        if (!self.tables_.Contains(meta_type))
            Log(Error) << "没有注册表类型";
        const auto &table = self.tables_[meta_type];
        table->Update(meta);
    }
}
