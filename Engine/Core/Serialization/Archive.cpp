/**
 * @file Archive.cpp
 * @author Echo 
 * @Date 24-10-24
 * @brief 
 */

#include "Archive.h"

#include "Log/CoreLogCategory.h"

#define VALUE_GETTER(type)                \
    else if (value.is_type<type>())       \
    {                                     \
        *this << value.get_value<type>(); \
    }

// TODO: 完成这个
// static void SerializeSequenceView(Archive& ar, const rttr::variant_sequential_view& view)
// {
//     ar << Archive::InputType::ArrayStart;
//     for (const auto& item: view)
//     {
//         if (item.is_sequential_container())
//         {
//             SerializeSequenceView(ar, item.create_sequential_view());
//         }
//         else
//         {
//             rttr::variant wrapped_var = item.extract_wrapped_value();
//             if (wrapped_var.get_type().is_pointer())
//             {
//                 // LOG_WARNING_ANSI_CATEGORY(Archive.Serialization, "可不序列化原始指针");
//                 continue;
//             }
//             ar << wrapped_var;
//         }
//     }
//     ar << Archive::InputType::ArrayEnd;
// }
//
// static void SerializeMapView(Archive& ar, const rttr::variant_associative_view& view)
// {
//     ar << Archive::InputType::MapStart;
//     for (const auto& item: view)
//     {
//         if (item.first.is_associative_container() || item.first.is_sequential_container())
//         {
//             // LOG_WARNING_ANSI_CATEGORY(Archive.Serialization, "键不可为容器");
//             continue;
//         }
//         ar << Archive::InputType::Key;
//         ar << item.first;
//         ar << Archive::InputType::Value;
//         ar << item.second;
//     }
//     ar << Archive::InputType::MapEnd;
// }

// Archive& Archive::operator<<(const rttr::variant& value)
// {
    // // Assert(Archive.Serialization, value.is_valid(), "序列化值无效");
    // if (value.is_type<int8_t>())
    // {
    //     *this << value.get_value<int8_t>();
    // }
    // VALUE_GETTER(int16_t)
    // VALUE_GETTER(int32_t)
    // VALUE_GETTER(int64_t)
    // VALUE_GETTER(uint8_t)
    // VALUE_GETTER(uint16_t)
    // VALUE_GETTER(uint32_t)
    // VALUE_GETTER(uint64_t)
    // VALUE_GETTER(float)
    // VALUE_GETTER(double)
    // VALUE_GETTER(bool)
    // VALUE_GETTER(const char*)
    // VALUE_GETTER(char*)
    // else if (value.get_type() == TypeOf<core::String>())
    // {
    //     const auto& value1 = value.get_value<core::String>();
    //     *this << value1;
    // }
    // else if (value.get_type().is_enumeration())
    // {
    //     const auto enum_name = value.get_type().get_enumeration().value_to_name(value);
    //     StringView view = {enum_name.data(), static_cast<int32_t>(enum_name.length())};
    //     *this << view;
    // }
    // else if (value.is_sequential_container())
    // {
    //     const auto& view = value.create_sequential_view();
    //     SerializeSequenceView(*this, view);
    // }
    // else if (value.is_associative_container())
    // {
    //     const auto& view = value.create_associative_view();
    //     SerializeMapView(*this, view);
    // }
    // else
    // {
    //     const auto type_name = value.get_type().get_name();
    //     StringView view      = {type_name.data(), static_cast<int32_t>(type_name.length())};
    //     LOGGER.Warn(LogCat::Archive_Serialization, "无法序列化的类型: {}", view);
    // }
    // return *this;
// }

