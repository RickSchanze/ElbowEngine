//
// Created by Echo on 2025/3/26.
//
#include "SQLHelper.hpp"

#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>

#include "Core/Misc/Other.hpp"
#include "Core/Profile.hpp"

String MapTypeToSqlType(const Type *type) {
    Assert(refl_helper::IsPrimitive(type) || type->IsEnumType(), "type必须为Primitive类型");
    Assert(type != TypeOf<StringView>(), "StringView为视图类型, 不可用作表字段类型");
    if (type == TypeOf<String>()) {
        return "TEXT";
    }
    if (type == TypeOf<float>() || type == TypeOf<double>()) {
        return "REAL";
    }
    return "INTEGER";
}

UniquePtr<SQLTable> CreateTypeTable(SQLite::Database &db, StringView name, const Type *type) {
    ProfileScope _(__func__);
    // 第一步, 验证type字段是否合法
    for (const auto fields = type->GetFields(); const auto &field_info: fields) {
        if (!(field_info->IsPrimitive() || field_info->GetType()->IsEnumType())) {
            const auto msg = String::Format("表类型{}的所有字段必须都为Primitive, 但字段{}的类型为{}", *name, *field_info->GetName(),
                                            *field_info->GetType()->GetName());
            Log(Fatal) << *msg;
        }
    }
    // 第二步, 构建SQL语句
    String sql_execute;
    struct FieldWithAttr {
        const Field *field_info;
        nlohmann::json attr;
    };
    Array<FieldWithAttr> collected_fields;
    // 1. 找主键
    {
        bool has_primary_key{};
        for (const auto fields = type->GetFields(); auto &field_info: fields) {
            nlohmann::json sql_attr = {};
            if (field_info->IsDefined(Field::ValueAttribute::SQLAttr)) {
                const auto sql_attr_str = field_info->GetAttribute(Field::ValueAttribute::SQLAttr);
                sql_attr = ParseSubAttr(sql_attr_str);
                if (sql_attr.contains("PrimaryKey")) {
                    has_primary_key = true;
                }
            }
            collected_fields.Add({field_info, sql_attr});
        }
        if (!has_primary_key) {
            Log(Fatal) << String::Format("表类型{}没有指定PrimaryKey", *name);
        }
    }
    // 2. 构建所有字段的SQL语句
    String new_sql = "(";
    for (size_t i = 0; i < collected_fields.Count(); ++i) {
        auto &[field_info, attr] = collected_fields[i];
        new_sql += String::Format("{} {} ", *field_info->GetName(), *MapTypeToSqlType(field_info->GetType()));
        if (!attr.contains("Nullable")) {
            new_sql += "NOT NULL ";
        }
        if (attr.contains("PrimaryKey")) {
            new_sql += "PRIMARY KEY ";
            if (!attr.contains("ManualPrimaryKey")) {
                new_sql += "AUTOINCREMENT ";
            }
        }
        if (i != collected_fields.Count() - 1) {
            new_sql += ", \n";
        }
    }
    new_sql += ");";
    sql_execute = String::Format("CREATE TABLE IF NOT EXISTS {} {}", *name, *new_sql);
    // 第三歩 执行
    db.exec(sql_execute);
    // 第四歩, 向TypeMeta中插入数据
    String sql_insert = String::Format("INSERT INTO {} (table_name, type_name, type_hash) VALUES (?, ?, ?);", *SQLHelper::GetTypeMetaTableName());
    SQLite::Statement insert(db, sql_insert);
    insert.bind(1, *name);
    insert.bind(2, *type->GetName());
    // 这里插入
    insert.bind(3, static_cast<int64_t>(type->GetHashCode()));
    insert.exec();
    return MakeUnique<SQLTable>(type, &db, name);
}

void SQLTable::Insert(const Any &data) {
    ProfileScope _(__func__);
    const auto type = data.GetType();
    if (type != type_) {
        Log(Fatal) << "data:"_es + "输入类型不符";
    }
    String position_argument = "(";
    String placeholder_argument = "(";
    const auto fields = type->GetFields();
    for (size_t i = 0; i < fields.Count(); ++i) {
        const auto &field_info = fields[i];
        position_argument += field_info->GetName();
        placeholder_argument += "?";
        if (i != fields.Count() - 1) {
            position_argument += ", ";
            placeholder_argument += ", ";
        }
    }
    position_argument += ")";
    placeholder_argument += ")";
    SQLite::Statement insert(*db_, String::Format("INSERT INTO {} {} VALUES {};", *table_name_, *position_argument, *placeholder_argument));
    for (size_t i = 0; i < fields.Count(); ++i) {
        const auto &field_info = fields[i];
        if (field_info->IsDefined(Field::ValueAttribute::SQLAttr)) {
            auto attr_str = field_info->GetAttribute(Field::ValueAttribute::SQLAttr);
            auto attr_map = ParseSubAttr(attr_str);
            if (attr_map.contains("PrimaryKey")) {
                continue;
            }
        }
        if (const auto &field_type = field_info->GetType();
            field_type == TypeOf<bool>() || field_type == TypeOf<int8_t>() || field_type == TypeOf<int16_t>() || field_type == TypeOf<int32_t>() ||
            field_type == TypeOf<int64_t>() || field_type == TypeOf<uint8_t>() || field_type == TypeOf<uint16_t>() ||
            field_type == TypeOf<uint32_t>() || field_type == TypeOf<uint64_t>()) {
            auto op = refl_helper::GetValue(field_info, data).AsInt64();
            if (!op) {
                Log(Fatal) << "存储类型错误";
            }
            insert.bind(static_cast<int32_t>(i) + 1, *op);
        } else if (field_type == TypeOf<String>()) {
            const auto value = refl_helper::GetValue(field_info, data).As<String>();
            if (value == nullptr) {
                Log(Fatal) << "存储类型错误";
            }
            insert.bind(static_cast<int32_t>(i) + 1, *value);
        } else if (field_type->IsEnumType()) {
            auto value = refl_helper::GetObjEnumValue(field_info, data);
            insert.bind(static_cast<int32_t>(i) + 1, *value);
        } else {
            Log(Fatal) << "存储类型错误";
        }
    }
    try {
        insert.exec();
    } catch (std::exception &e) {
        Log(Fatal) << "SqlLite异常: "_es + e.what();
    }
}

Array<SharedAny> SQLTable::Query(const Type *type, StringView where) {
    if (type_ != type) {
        Log(Fatal) << "输入类型不符";
    }
    // 1. 构建字段语句
    String field_stat;
    const auto fields = type->GetFields();
    for (int i = 0; i < fields.Count(); ++i) {
        field_stat += fields[i]->GetName();
        if (i != fields.Count() - 1) {
            field_stat += ", ";
        }
    }
    String query_stat = String::Format("SELECT {} FROM {}", *field_stat, *table_name_);
    if (!where.IsEmpty()) {
        query_stat += String::Format(" WHERE {}", *where);
    }
    try {
        Array<SharedAny> results;
        SQLite::Statement query(*db_, query_stat);
        while (query.executeStep()) {
            SharedAny result(type);
            Any temp = result.AsAny();
            for (int i = 0; i < fields.Count(); ++i) {
                auto &field = fields[i];
                if (refl_helper::IsNumericInteger(field->GetType())) {
                    const int64_t value = query.getColumn(i).getInt64();
                    Any any_value = Any{&value, TypeOf<Int64>()};
                    refl_helper::SetValue(field, temp, any_value);
                    continue;
                }
                if (refl_helper::IsNumericFloat(field->GetType())) {
                    const double value = query.getColumn(i).getDouble();
                    Any any_value = Any{&value, TypeOf<Double>()};
                    refl_helper::SetValue(field, temp, any_value);
                    continue;
                }
                if (field->GetType() == TypeOf<Bool>()) {
                    const int64_t value = query.getColumn(i).getInt64();
                    bool v = value != 0;
                    Any any_value = Any{&v, TypeOf<Bool>()};
                    refl_helper::SetValue(field, temp, any_value);
                    continue;
                }
                if (field->GetType() == TypeOf<String>()) {
                    const String value = query.getColumn(i).getString();
                    Any any_value = Any{&value, TypeOf<String>()};
                    refl_helper::SetValue(field, temp, any_value);
                    continue;
                }
                if (field->GetType()->IsEnumType()) {
                    const Int32 value = query.getColumn(i).getInt64();
                    Any any_value = Any{&value, TypeOf<Int32>()};
                    refl_helper::SetValue(field, temp, any_value);
                    continue;
                }
                Log(Error) << "查询类型错误";
                return {};
            }
            results.Add(result);
        }
        return results;
    } catch (std::exception &e) {
        VLOG_FATAL("Sql异常, 查询语句为: ", query_stat, ", 异常信息为", e.what());
        Exit(1);
        return {};
    }
}

UniquePtr<SQLTable> SQLHelper::CreateTable(SQLite::Database &db, const Type *type, const bool allow_exist) {
    ProfileScope _(__func__);
    if (type == nullptr) {
        Log(Fatal) << "type为空";
    }
    if (!type->IsDefined(Type::ValueAttribute::SQLTable)) {
        Log(Fatal) << "type必须被SQLTable标记";
    }
    StringView table_name = type->GetName();
    if (!type->GetAttributeValue(Type::ValueAttribute::SQLTable).IsEmpty()) {
        table_name = type->GetAttributeValue(Type::ValueAttribute::SQLTable);
    }
    if (db.tableExists(table_name.Data())) {
        if (!allow_exist) {
            const String msg = String::Format("表{}已存在.", table_name);
            Log(Fatal) << msg;
        }
        return MakeUnique<SQLTable>(type, AddressOf(db), table_name);
    }
    return CreateTypeTable(db, table_name, type);
}

void SQLHelper::InitializeDataBase(SQLite::Database &db) {
    auto sql_str = String::Format(
            R"(CREATE TABLE IF NOT EXISTS {} (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    table_name TEXT NOT NULL,
    type_name TEXT NOT NULL,
    type_hash INTEGER NOT NULL
);)",
            *GetTypeMetaTableName());
    db.exec(sql_str.Data());
}

StringView SQLHelper::GetTypeMetaTableName() { return "__TYPE_META__"; }
