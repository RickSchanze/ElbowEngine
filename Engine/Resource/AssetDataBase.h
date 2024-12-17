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

protected:
    void CreateAssetTables();

    SQLite::Database* db_ = nullptr;
    core::HashMap<const core::Type*, core::resource::SQLTable> tables_;
};
}   // namespace resource
