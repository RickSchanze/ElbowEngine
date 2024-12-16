//
// Created by Echo on 24-12-16.
//

#include "SQLHelper.h"

#include "Core/Base/Exception.h"

static void CreateTypeTable(SQLite::Database& db, core::StringView name, const core::Type* type) {}

void core::resource::SQLHelper::CreateTable(SQLite::Database& db, const Type* type)
{
    if (type == nullptr)
    {
        throw ArgumentException(NAMEOF(type), "输入为空");
    }
    if (!type->IsDefined(Type::ValueAttribute::SQLTable))
    {
        throw ArgumentException(NAMEOF(type), "输入type必须被SQLTable标记");
    }
    core::StringView table_name = type->GetName();
    if (!type->IsAttributeValueNull(Type::ValueAttribute::SQLTable))
    {
        table_name = type->GetAttributeValue(Type::ValueAttribute::SQLTable);
    }
    CreateTypeTable(db, table_name, type);
}