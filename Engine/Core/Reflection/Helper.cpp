//
// Created by Echo on 2025/3/24.
//

#include "Helper.hpp"

#include "Core/Logger/Logger.hpp"
#include "Core/Memory/Memory.hpp"
#include "ReflManager.hpp"

static void CastInteger(void *target, const Type *t, Int64 v);

static void CastFloat(void *target, const Type *t, Double v);

bool NReflHelper::IsPrimitive(const Type *t) {
  if (t == TypeOf<Int8>())
    return true;
  if (t == TypeOf<UInt16>())
    return true;
  if (t == TypeOf<UInt32>())
    return true;
  if (t == TypeOf<UInt64>())
    return true;
  if (t == TypeOf<Int8>())
    return true;
  if (t == TypeOf<Int16>())
    return true;
  if (t == TypeOf<Int32>())
    return true;
  if (t == TypeOf<Int64>())
    return true;
  if (t == TypeOf<Float>())
    return true;
  if (t == TypeOf<Double>())
    return true;
  if (t == TypeOf<Bool>())
    return true;
  if (t == TypeOf<String>())
    return true;
  if (t == TypeOf<StringView>())
    return true;
  return false;
}

Any NReflHelper::GetValue(const Field *field, const Any &obj) {
    if (!obj.HasValue() || !field) {
        Log(Error) << "obj or field is null";
        return {};
    }
    if (!obj.IsDerivedFrom(field->outer)) {
        Log(Error) << String::Format("Different outer type, obj type: {}, outer type: {}", *obj.GetType()->GetName(), *field->outer->GetName());
        return {};
    }
    return {field->GetFieldPtr(obj.GetRawPtr()), field->GetType()};
}

Optional<Int32> NReflHelper::GetObjEnumValue(const Field *field, const Any &obj) {
    const auto value = GetValue(field, obj);
    return value.AsCopy<Int32>();
}

bool NReflHelper::SetValue(const Field *field, const Any &obj, const Any &value) {
  if (!obj.HasValue() || !value.HasValue()) {
    Log(Error) << "value或者obj没有设置值";
    return false;
  }
  if (obj.GetType() != field->outer) {
    Log(Error) << "obj类型错误";
    return false;
  }
  if (field->IsSequentialContainer() || field->IsAssociativeContainer()) {
    Log(Fatal) << "不支持直接设置容器的值";
  }
  if (IsNumericInteger(field->type)) {
    const Int64 v = *value.AsInt64();
    CastInteger(field->GetFieldPtr(obj.GetRawPtr()), field->type, v);
    return true;
  }
  if (field->type == TypeOf<bool>()) {
    if (value.GetType() == TypeOf<bool>()) {
      *static_cast<bool *>(field->GetFieldPtr(obj.GetRawPtr())) = *static_cast<bool *>(value.GetRawPtr());
      return true;
    }
    if (IsNumericInteger(value.GetType())) {
      const Int64 v = *value.AsInt64();
      *static_cast<bool *>(field->GetFieldPtr(obj.GetRawPtr())) = v != 0;
      return true;
    }
    Log(Error) << "bool只能由bool或者整数类型转换而来";
    return false;
  }
  if (IsNumericFloat(field->type)) {
    const double v = *value.AsDouble();
    CastFloat(field->GetFieldPtr(obj.GetRawPtr()), field->type, v);
    return true;
  }
  if (field->type == TypeOf<String>()) {
    *static_cast<String *>(field->GetFieldPtr(obj.GetRawPtr())) = *static_cast<String *>(value.GetRawPtr());
    return true;
    ;
  }
  if (field->type->IsEnumType()) {
    *static_cast<Int32 *>(field->GetFieldPtr(obj.GetRawPtr())) = *static_cast<Int32 *>(value.GetRawPtr());
    return true;
  }
  if (value.GetType() != field->type) {
    Log(Error) << "类型不匹配";
    return false;
  }
  // TODO: memcpy有时候会比较危险, Field是指针的情况下
  Memcpy(field->GetFieldPtr(obj.GetRawPtr()), value.GetRawPtr(), field->type->GetSize());
  return true;
}

void CastInteger(void *target, const Type *t, Int64 v) {
  if (t == TypeOf<Int8>())
    *static_cast<Int8 *>(target) = static_cast<Int8>(v);
  if (t == TypeOf<UInt8>())
    *static_cast<UInt8 *>(target) = static_cast<UInt8>(v);
  if (t == TypeOf<Int16>())
    *static_cast<Int16 *>(target) = static_cast<Int16>(v);
  if (t == TypeOf<UInt16>())
    *static_cast<UInt16 *>(target) = static_cast<UInt16>(v);
  if (t == TypeOf<Int32>())
    *static_cast<Int32 *>(target) = static_cast<Int32>(v);
  if (t == TypeOf<UInt32>())
    *static_cast<UInt32 *>(target) = static_cast<UInt32>(v);
  if (t == TypeOf<Int64>())
    *static_cast<Int64 *>(target) = static_cast<Int64>(v);
  if (t == TypeOf<UInt64>())
    *static_cast<UInt64 *>(target) = static_cast<UInt64>(v);
}

void CastFloat(void *target, const Type *t, const double v) {
  if (t == TypeOf<float>())
    *static_cast<float *>(target) = static_cast<float>(v);
  if (t == TypeOf<double>())
    *static_cast<double *>(target) = static_cast<double>(v);
}

Ext_AddEnumField::Ext_AddEnumField(const StringView name, Int32 value, const Type *outer_t) {
  field = New<Field>();
  field->name = name;
  field->outer = outer_t;
  field->size = sizeof(Int32);
  field->offset = 0;
  field->type = TypeOf<Int32>();
  field->enum_value = value;
}
