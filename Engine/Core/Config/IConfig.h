/**
 * @file IConfig.h
 * @author Echo 
 * @Date 24-11-16
 * @brief 
 */

#pragma once
#include "Core/Core.h"

#include GEN_HEADER("Core.IConfig.generated.h")
#include "Core/Base/Interface.h"
#include "Core/Object/Object.h"

#ifdef WITH_EDITOR
#define DECLARE_CONFIG_ITEM(type, field_name, name, ...) \
private:                                                 \
    PROPERTY()                                           \
    type field_name = __VA_ARGS__;                       \
                                                         \
public:                                                  \
    [[nodiscard]] const type& Get##name() const          \
    {                                                    \
        return field_name;                               \
    }                                                    \
    void Set##name(const type& value)                    \
    {                                                    \
        if (field_name != value)                         \
        {                                                \
            field_name = value;                          \
            SetDirty(true);                              \
        }                                                \
    }

#define DECLARE_CONFIG_ITEM_MAP(type1, type2, field_name, name, ...) \
    PROPERTY()                                                       \
    type1, type2 field_name = __VA_ARGS__;                           \
                                                                     \
public:                                                              \
    [[nodiscard]] const type1, type2& Get##name() const              \
    {                                                                \
        return field_name;                                           \
    }                                                                \
    void Set##name(const type1, type2& value)                        \
    {                                                                \
        if (field_name != value)                                     \
        {                                                            \
            field_name = value;                                      \
            SetDirty(true);                                          \
        }                                                            \
    }
#else
#define DECLARE_CONFIG_ITEM(type, field_name, name, ...) \
private:                                                 \
    PROPERTY()                                           \
    type field_name = type(__VA_ARGS__);                 \
                                                         \
public:                                                  \
    [[nodiscard]] const type& Get##name() const          \
    {                                                    \
        return field_name;                               \
    }                                                    \
    void Set##name(const type& value) {}

#define DECLARE_CONFIG_ITEM_MAP(type1, type2, field_name, name, ...) \
    PROPERTY()                                                       \
    type1, type2 field_name = __VA_ARGS__;                           \
                                                                     \
public:                                                              \
    [[nodiscard]] const type1, type2& Get##name() const              \
    {                                                                \
        return field_name;                                           \
    }                                                                \
    void Set##name(const type1, type2& value) {}

#endif

namespace core
{
class CLASS(Interface) IConfig : extends Interface
{
    GENERATED_CLASS(IConfig)

public:
    void               SetDirty(bool dirty);
    [[nodiscard]] bool IsDirty() const { return dirty_; }

protected:
    bool dirty_ = false;
};
}   // namespace core
