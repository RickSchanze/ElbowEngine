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
    typedef core::StaticArray<core::UniquePtr<SQLite::Database>, GetEnumValue(AssetType::Count)> DataBaseArray;

    [[nodiscard]] core::ManagerLevel GetLevel() const override { return core::ManagerLevel::Top; }
    [[nodiscard]] core::StringView   GetName() const override { return "AssetDataBase"; }

    /**
     * 这里会打开不同种类的数据库连接
     * 如果还没有, 则会创建这些数据库连接
     */
    void Startup() override;

protected:
    DataBaseArray databases_;
};
}   // namespace resource
