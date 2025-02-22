//
// Created by Echo on 24-12-16.
//

#include "SQLHelper.h"

#include "Core/Base/Exception.h"
#include "Core/Profiler/ProfileMacro.h"
#include "Logcat.h"

#include "SQLiteCpp/SQLiteCpp.h"
using namespace core;
using namespace resource;

static String MapTypeToSqlType(const Type *type) {
  Assert::Require(logcat::Resource_AssetDataBase, type->IsPrimitive() || type->IsEnum(), "type必须为Primitive类型");
  Assert::Require(logcat::Resource_AssetDataBase, type != TypeOf<StringView>(),
                  "StringView为视图类型, 不可用作表字段类型");
  if (type == TypeOf<String>()) {
    return "TEXT";
  }
  if (type == TypeOf<float>() || type == TypeOf<double>()) {
    return "REAL";
  }
  return "INTEGER";
}

static UniquePtr<SQLTable> CreateTypeTable(SQLite::Database &db, StringView name, const Type *type) {
  PROFILE_SCOPE_AUTO;
  // 第一步, 验证type字段是否合法
  for (const auto fields = type->GetFields(); const auto &field_info : fields) {
    if (!(field_info->GetType()->IsPrimitive() || field_info->GetType()->IsEnum())) {
      const auto msg = String::Format("表类型{}的所有字段必须都为Primitive, 但字段{}的类型为{}", name,
                                      field_info->GetName(), field_info->GetType()->GetName());
      throw SQLException(msg);
    }
  }
  // 第二步, 构建SQL语句
  String sql_execute;
  struct FieldWithAttr {
    Ref<const FieldInfo> field_info;
    nlohmann::json attr;
  };
  Array<FieldWithAttr> collected_fields;
  // 1. 找主键
  {
    bool has_primary_key{};
    for (const auto fields = type->GetFields(); auto &field_info : fields) {
      nlohmann::json sql_attr = {};
      if (field_info->IsDefined(FieldInfo::ValueAttribute::SQLAttr)) {
        const auto sql_attr_str = field_info->GetAttribute(FieldInfo::ValueAttribute::SQLAttr);
        sql_attr = ParseSubAttr(sql_attr_str);
        if (sql_attr.contains("PrimaryKey")) {
          has_primary_key = true;
        }
      }
      collected_fields.emplace_back(field_info, sql_attr);
    }
    if (!has_primary_key) {
      throw SQLException(String::Format("表类型{}没有指定PrimaryKey", name));
    }
  }
  // 2. 构建所有字段的SQL语句
  String new_sql = "(";
  for (size_t i = 0; i < collected_fields.size(); ++i) {
    auto &[field_info, attr] = collected_fields[i];
    new_sql += String::Format("{} {} ", field_info->GetName(), MapTypeToSqlType(field_info->GetType()));
    if (!attr.contains("Nullable")) {
      new_sql += "NOT NULL ";
    }
    if (attr.contains("PrimaryKey")) {
      new_sql += "PRIMARY KEY ";
      if (!attr.contains("ManualPrimaryKey")) {
        new_sql += "AUTOINCREMENT ";
      }
    }
    if (i != collected_fields.size() - 1) {
      new_sql += ", \n";
    }
  }
  new_sql += ");";
  sql_execute = String::Format("CREATE TABLE IF NOT EXISTS {} {}", name, new_sql);
  // 第三歩 执行
  db.exec(sql_execute);
  // 第四歩, 向TypeMeta中插入数据
  String sql_insert = String::Format("INSERT INTO {} (table_name, type_name, type_hash) VALUES (?, ?, ?);",
                                     SQLHelper::GetTypeMetaTableName());
  SQLite::Statement insert(db, sql_insert);
  insert.bind(1, name.Data());
  insert.bind(2, type->GetName().Data());
  // 这里插入
  insert.bind(3, static_cast<int64_t>(type->GetTypeHash()));
  insert.exec();
  return MakeUnique<SQLTable>(type, &db, name);
}

void SQLTable::Insert(const Any &data) {
  PROFILE_SCOPE_AUTO;
  const auto type = data.GetType();
  if (type != type_) {
    throw ArgumentException(NAMEOF(data), "输入类型不符");
  }
  String position_argument = "(";
  String placeholder_argument = "(";
  const auto fields = type->GetFields();
  for (size_t i = 0; i < fields.size(); ++i) {
    const auto &field_info = fields[i];
    position_argument += field_info->GetName();
    placeholder_argument += "?";
    if (i != fields.size() - 1) {
      position_argument += ", ";
      placeholder_argument += ", ";
    }
  }
  position_argument += ")";
  placeholder_argument += ")";
  SQLite::Statement insert(
      *db_, String::Format("INSERT INTO {} {} VALUES {};", table_name_, position_argument, placeholder_argument));
  for (size_t i = 0; i < fields.size(); ++i) {
    const auto &field_info = fields[i];
    if (field_info->IsDefined(FieldInfo::ValueAttribute::SQLAttr)) {
      auto attr_str = field_info->GetAttribute(FieldInfo::ValueAttribute::SQLAttr);
      auto attr_map = ParseSubAttr(attr_str);
      if (attr_map.contains("PrimaryKey")) {
        continue;
      }
    }
    if (const auto &field_type = field_info->GetType();
        field_type == TypeOf<bool>() || field_type == TypeOf<int8_t>() || field_type == TypeOf<int16_t>() ||
        field_type == TypeOf<int32_t>() || field_type == TypeOf<int64_t>() || field_type == TypeOf<uint8_t>() ||
        field_type == TypeOf<uint16_t>() || field_type == TypeOf<uint32_t>() || field_type == TypeOf<uint64_t>()) {
      auto op = field_info->GetValue(data).AsInt64();
      if (!op) {
        throw SQLException("存储类型错误");
      }
      insert.bind(static_cast<int32_t>(i) + 1, *op);
    } else if (field_type == TypeOf<String>()) {
      const auto value = field_info->GetValue(data).As<String>();
      if (value == nullptr) {
        throw SQLException("存储类型错误");
      }
      insert.bind(static_cast<int32_t>(i) + 1, *value);
    } else if (field_type->IsEnum()) {
      auto value = field_info->GetObjEnumValue(data);
      insert.bind(static_cast<int32_t>(i) + 1, *value);
    } else {
      throw SQLException("存储类型错误");
    }
  }
  try {
    insert.exec();
  } catch (std::exception &e) {
    throw SQLException(e.what());
  }
}

UniquePtr<SQLTable> SQLHelper::CreateTable(Ref<SQLite::Database> db, const Type *type, bool allow_exist) {
  PROFILE_SCOPE_AUTO;
  if (type == nullptr) {
    throw ArgumentException(NAMEOF(type), "输入为空");
  }
  if (!type->IsDefined(Type::ValueAttribute::SQLTable)) {
    throw ArgumentException(NAMEOF(type), "输入type必须被SQLTable标记");
  }
  StringView table_name = type->GetName();
  if (!type->IsAttributeValueNull(Type::ValueAttribute::SQLTable)) {
    table_name = type->GetAttributeValue(Type::ValueAttribute::SQLTable);
  }
  if (db->tableExists(table_name.Data())) {
    if (!allow_exist) {
      const String msg = String::Format("表{}已存在.", table_name);
      throw SQLException(msg);
    }
    return MakeUnique<SQLTable>(type, db.GetPtr(), table_name);
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
      GetTypeMetaTableName());
  db.exec(sql_str.Data());
}

StringView SQLHelper::GetTypeMetaTableName() { return "__TYPE_META__"; }

Array<SharedAny> SQLTable::Query(const Type *type, StringView where) {
  if (type_ != type) {
    throw ArgumentException(NAMEOF(type), "输入类型不符");
  }
  Array<SharedAny> results;
  // 1. 构建字段语句
  String field_stat;
  const auto fields = type->GetFields();
  for (int i = 0; i < fields.size(); ++i) {
    field_stat += fields[i]->GetName();
    if (i != fields.size() - 1) {
      field_stat += ", ";
    }
  }
  String query_stat = String::Format("SELECT {} FROM {}", field_stat, table_name_);
  if (!where.IsEmpty()) {
    query_stat += String::Format(" WHERE {}", where);
  }
  try {
    SQLite::Statement query(*db_, query_stat);
    while (query.executeStep()) {
      SharedAny result(type);
      Any temp = result.AsAny();
      for (int i = 0; i < fields.size(); ++i) {
        auto &field = fields[i];
        if (field->GetType()->IsNumericInteger()) {
          const int64_t value = query.getColumn(i).getInt64();
          Any any_value = Any{&value, TypeOf<Int64>()};
          field->SetValue(temp, any_value);
          continue;
        }
        if (field->GetType()->IsNumericFloat()) {
          const double value = query.getColumn(i).getDouble();
          Any any_value = Any{&value, TypeOf<Double>()};
          field->SetValue(temp, any_value);
          continue;
        }
        if (field->GetType()->IsBoolean()) {
          const int64_t value = query.getColumn(i).getInt64();
          bool v = value != 0;
          Any any_value = Any{&v, TypeOf<Bool>()};
          field->SetValue(temp, any_value);
          continue;
        }
        if (field->GetType()->IsString()) {
          const String value = query.getColumn(i).getString();
          Any any_value = Any{&value, TypeOf<String>()};
          field->SetValue(temp, any_value);
          continue;
        }
        if (field->GetType()->IsEnum()) {
          const Int32 value = query.getColumn(i).getInt64();
          Any any_value = Any{&value, TypeOf<Int32>()};
          field->SetValue(temp, any_value);
          continue;
        }
        throw SQLException("查询类型错误");
      }
      results.emplace_back(result);
    }
    return results;
  } catch (std::exception &e) {
    auto w = e.what();
    auto w1 = e.what();
  }

}