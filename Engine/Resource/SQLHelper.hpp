//
// Created by Echo on 2025/3/23.
//
#pragma once
#include "Core/Core.hpp"
#include "Core/Misc/UniquePtr.hpp"
#include "Core/Reflection/SharedAny.hpp"

namespace SQLite {
    class Database;
}

class SQLTable;

static String MapTypeToSqlType(const Type *type);

static UniquePtr<SQLTable> CreateTypeTable(SQLite::Database &db, StringView name, const Type *type);

class SQLTable {
public:
    SQLTable() = default;
    SQLTable(const Type *type, SQLite::Database *db, StringView table_name) : type_(type), db_(db), table_name_(table_name) {}
    SQLTable(const SQLTable &other) = default;

    void Insert(const Any &data);
    void Update(const Any& data);

    /**
     * 查询特定条件的数据
     * @param type 用于校验
     * @param where 查询条件
     * @return
     */
    Array<SharedAny> Query(const Type *type, StringView where = "");

private:
    const Type *type_ = nullptr;
    SQLite::Database *db_ = nullptr;
    StringView table_name_{};
};

class SQLHelper {
public:
    /**
     * 在数据库db中创建一个表
     * @note 如果有两个主键, 则使用第一个
     *       如果没有主键, 抛出SQLException
     * @param db 数据库
     * @param type 表字段类型, 自动创建
     * @param allow_exist 允许表存在 为false时, 如果表存在, 抛出异常
     */
    static UniquePtr<SQLTable> CreateTable(SQLite::Database &db, const Type *type, const bool allow_exist = true);

    /**
     * 初始化db 创建一个Meta表用于存储表名和类型的关系
     * @param db
     */
    static void InitializeDataBase(SQLite::Database &db);

    static StringView GetTypeMetaTableName();
};
