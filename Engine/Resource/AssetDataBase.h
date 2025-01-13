/**
 * @file AssetDataBase.h
 * @author Echo 
 * @Date 24-12-4
 * @brief 
 */

#pragma once
#include "Core/Base/Base.h"
#include "Core/Singleton/MManager.h"
#include "SQLHelper.h"

namespace SQLite
{
class Database;
}

namespace resource
{

class AssetDataBase : public core::Manager<AssetDataBase>
{
public:
    [[nodiscard]] core::ManagerLevel GetLevel() const override { return core::ManagerLevel::L8; }
    [[nodiscard]] core::StringView   GetName() const override { return "AssetDataBase"; }

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
    static core::exec::AsyncResultHandle<core::ObjectHandle> Import(core::StringView path);

    /**
     * 加载一个资产
     * @param path
     * @return
     */
    static core::Object* Load(core::StringView path);

    template <typename T> requires std::derived_from<T, core::Object>
    static T* Load(core::StringView path) { return static_cast<T*>(Load(path)); }

    static core::exec::AsyncResultHandle<core::ObjectHandle> LoadAsync(core::StringView path);

    template <typename T>
    static core::Optional<T> QueryMeta(core::ObjectHandle handle);

    template <typename T>
    static core::Optional<T> QueryMeta(core::StringView where);

    template <typename T>
    static void InsertMeta(const T& meta);

protected:
    void CreateAssetTables();

    SQLite::Database*                                                           db_ = nullptr;
    core::HashMap<const core::Type*, core::UniquePtr<core::resource::SQLTable>> tables_;
};

template <typename T>
core::Optional<T> AssetDataBase::QueryMeta(core::ObjectHandle handle)
{
    core::String query = core::String::Format("object_handle = {}", handle);
    return QueryMeta<T>(query);
}

template <typename T>
core::Optional<T> AssetDataBase::QueryMeta(core::StringView where)
{
    auto&             self      = GetByRef();
    const core::Type* meta_type = core::TypeOf<T>();
    if (!self.tables_.contains(meta_type)) return {};
    auto& table  = self.tables_[meta_type];
    auto  result = table->Query(meta_type, where);
    if (result.empty()) return {};
    auto meta = result[0];
    return meta.AsAny().AsCopy<T>();
}

template <typename T>
void AssetDataBase::InsertMeta(const T& meta)
{
    const core::Type* meta_type = core::TypeOf<T>();
    if (!Get()->tables_.contains(meta_type)) throw core::ArgumentException(NAMEOF(meta), "没有注册表类型");
    const auto& table = Get()->tables_[meta_type];
    table->Insert(meta);
}

}   // namespace resource
