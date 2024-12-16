/**
 * @file AssetDataBase.h
 * @author Echo 
 * @Date 24-12-4
 * @brief 
 */

#pragma once
#include "AssetType.h"
#include "Core/Base/Base.h"
#include "Core/Base/UniquePtr.h"
#include "Core/Singleton/MManager.h"
#include "SQLiteCpp/SQLiteCpp.h"

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

protected:
    core::UniquePtr<SQLite::Database> db_;
};
}   // namespace resource
