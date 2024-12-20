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
    [[nodiscard]] core::ManagerLevel GetLevel() const override { return core::ManagerLevel::Top; }
    [[nodiscard]] core::StringView   GetName() const override { return "AssetDataBase"; }

    /**
     * 打开/创建资产数据库
     */
    void Startup() override;

    void Shutdown() override;


    template <typename T>
    core::Optional<T> QueryMeta(core::ObjectHandle handle);

    template <typename T>
    core::Optional<T> QueryMeta(core::StringView where);

protected:
    void CreateAssetTables();

    SQLite::Database*                                          db_ = nullptr;
    core::HashMap<const core::Type*, core::resource::SQLTable> tables_;
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
    const core::Type* meta_type = T::GetStaticMetaType();
    if (!tables_.contains(meta_type)) return {};
    auto& table  = tables_[meta_type];
    auto  result = table.Query(meta_type, where);
    if (result.empty()) return {};
    auto meta = result[0];
    return meta.AsAny().AsCopy<T>();
}

}   // namespace resource
