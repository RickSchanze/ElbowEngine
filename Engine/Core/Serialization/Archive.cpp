/**
 * @file Archive.cpp
 * @author Echo 
 * @Date 24-10-24
 * @brief 
 */

#include "Archive.h"

#include "Log/CoreLogCategory.h"
#include "Reflection/Any.h"
#include "Reflection/Reflection.h"

// 序列化基本类型, 不能是引用, 对于字符串
// const char* 和 char* 是不支持的
// 请使用String或者StringView
static void SerializeBasicType(core::Archive& ar, const core::Any& value)
{
    DebugAssert(LogCat::Archive_Serialization, value.HasValue(), "序列化值无效");
    auto value_type = value.GetType();
#define VALUE_GETTER(type)                                                                              \
    if (value_type == core::TypeOf<type>())                                                             \
    {                                                                                                   \
        auto v_op = any_cast<type>(value);                                                              \
        if (v_op.has_value())                                                                           \
        {                                                                                               \
            ar << v_op.value();                                                                         \
        }                                                                                               \
        else                                                                                            \
        {                                                                                               \
            LOGGER.Error(LogCat::Archive_Serialization, "序列化出错: {}", GetEnumString(v_op.error())); \
        }                                                                                               \
        return;                                                                                         \
    }
    VALUE_GETTER(int8_t)
    VALUE_GETTER(int16_t)
    VALUE_GETTER(int32_t)
    VALUE_GETTER(int64_t)
    VALUE_GETTER(uint8_t)
    VALUE_GETTER(uint16_t)
    VALUE_GETTER(uint32_t)
    VALUE_GETTER(uint64_t)
    VALUE_GETTER(float)
    VALUE_GETTER(double)
    VALUE_GETTER(bool)
    VALUE_GETTER(core::String)
    VALUE_GETTER(core::StringView)
#undef VALUE_GETTER
    LOGGER.Error(LogCat::Archive_Serialization, "序列化出错: 不支持的数据类型: {}", value_type->GetName());
}

static void SerializeBasicRefType(core::Archive& ar, const core::Any& value)
{
    DebugAssert(LogCat::Archive_Serialization, value.HasValue(), "序列化值无效");
    auto value_type = value.GetType();
#define VALUE_GETTER(type)                                                                              \
    if (value_type == core::TypeOf<type>())                                                             \
    {                                                                                                   \
        auto v_op = any_cast<core::Ref<type>>(value);                                                   \
        if (v_op.has_value())                                                                           \
        {                                                                                               \
            ar << *v_op.value();                                                                        \
        }                                                                                               \
        else                                                                                            \
        {                                                                                               \
            LOGGER.Error(LogCat::Archive_Serialization, "序列化出错: {}", GetEnumString(v_op.error())); \
        }                                                                                               \
        return;                                                                                         \
    }
    VALUE_GETTER(int8_t)
    VALUE_GETTER(int16_t)
    VALUE_GETTER(int32_t)
    VALUE_GETTER(int64_t)
    VALUE_GETTER(uint8_t)
    VALUE_GETTER(uint16_t)
    VALUE_GETTER(uint32_t)
    VALUE_GETTER(uint64_t)
    VALUE_GETTER(float)
    VALUE_GETTER(double)
    VALUE_GETTER(bool)
    VALUE_GETTER(core::String)
    VALUE_GETTER(core::StringView)
#undef VALUE_GETTER
    LOGGER.Error(LogCat::Archive_Serialization, "序列化出错: 不支持的引用数据类型: {}", value_type->GetName());
}

core::Archive& core::Archive::operator<<(const core::Any& value)
{
    DebugAssert(LogCat::Archive_Serialization, IsSerializing(), "请在Serializing模式使用此函数");
    if (!value.HasValue())
    {
        LOGGER.Error(LogCat::Archive_Serialization, "序列化值无效");
        SetError();
        return *this;
    }
    auto value_type = value.GetType();
    if (value_type->IsPrimitive())
    {
        if (value.IsRef())
        {
            SerializeBasicRefType(*this, value);
        }
        else
        {
            SerializeBasicType(*this, value);
        }
    }
    else
    {
        if (value.IsRef())
        {
            LOGGER.Error(LogCat::Archive_Serialization, "反序列化自定义类型的Ref是不支持的: Ref<{}>", value_type->GetName());
            SetError();
            return *this;
        }
        else
        {
            auto obj_op = any_cast<ITypeGetter*>(value);
            if (!obj_op.has_value())
            {
                LOGGER.Error(LogCat::Archive_Serialization, "基础类型外, 可序列化的类型必须是一个ITypeGetter");
                SetError();
                return *this;
            }
            const auto fields = value_type->GetFields();
            auto       obj    = obj_op.value();
            *this << InputType::MapStart;
            for (auto& field: fields)
            {
                *this << InputType::Key;
                *this << field->GetName();
                *this << InputType::Value;
                if (field->IsAssociativeContainer())
                {
                }
                else if (field->IsSequentialContainer())
                {
                }
                else
                {
                    auto any_op = field->GetAny(obj);
                    if (any_op.has_value())
                    {
                        *this << any_op.value();
                    }
                    else
                    {
                        LOGGER.Error(LogCat::Archive_Serialization, "序列化出错");
                        SetError();
                        return *this;
                    }
                }
            }
            *this << InputType::MapEnd;
        }
    }
    return *this;
}
