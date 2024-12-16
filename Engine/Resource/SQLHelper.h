//
// Created by Echo on 24-12-16.
//

#pragma once
#include "Core/Base/EString.h"
#include "Core/Reflection/Reflection.h"


#include <SQLiteCpp/Database.h>

namespace core::resource
{
class SQLHelper {
public:
    /**
     * 在数据库db中创建一个表
     * @param db 数据库
     * @param type 表字段类型, 自动创建
     */
    static void CreateTable(SQLite::Database& db, const Type* type);
};
}
