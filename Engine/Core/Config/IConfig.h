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

#if WITH_EDITOR
#define DECLARE_CONFIG_ITEM(type, field_name, name, ...)                \
    type field_name = __VA_ARGS__;                                      \
public:                                                                 \
    [[nodiscard]] const type& Get##name() const                         \
    {                                                                   \
        return field_name;                                              \
    }                                                                   \
    type& Get##name() { return field_name; }                            \
    void Set##name(const type& value)                                   \
    {                                                                   \
        if (field_name != value)                                        \
        {                                                               \
            field_name = value;                                         \
            SetDirty(true);                                             \
        }                                                               \
    } \
    private:

#define DECLARE_CONFIG_ITEM_MAP(type1, type2, field_name, name, ...)                \
    type1, type2 field_name = __VA_ARGS__;                                          \
                                                                                    \
public:                                                                             \
    [[nodiscard]] const type1, type2& Get##name() const                             \
    {                                                                               \
        return field_name;                                                          \
    }                                                                               \
    type1, type2& Get##name()                                                       \
    {                                                                               \
        return field_name;                                                          \
    }                                                                               \
    void Set##name(const type1, type2& value)                                       \
    {                                                                               \
        if (field_name != value)                                                    \
        {                                                                           \
            field_name = value;                                                     \
            SetDirty(true);                                                         \
        }                                                                           \
    }                                                                               \
    private:
#else
#define DECLARE_CONFIG_ITEM(type, field_name, name, ...)               \
private:                                                               \
    type field_name = __VA_ARGS__;                                     \
                                                                       \
public:                                                                \
    type& Get##name() { return field_name; }                           \
    [[nodiscard]] const type& Get##name() const                        \
    {                                                                  \
        return field_name;                                             \
    }                                                                  \
    type& Get##name() { return field_name; }                           \
    void Set##name(const type& value) {}                               \
    private:

#define DECLARE_CONFIG_ITEM_MAP(type1, type2, field_name, name, ...)               \
    type1, type2 field_name = __VA_ARGS__;                                         \
                                                                                   \
public:                                                                            \
    [[nodiscard]] const type1, type2& Get##name() const                            \
    {                                                                              \
        return field_name;                                                         \
    }                                                                              \
    type1, type2& Get##name()                                                      \
    {                                                                              \
        return field_name;                                                         \
    }                                                                              \
    void Set##name(const type1, type2& value) {}                                   \
    private:

#endif

namespace core
{
class CLASS(Interface) IConfig : extends Interface
{
    GENERATED_CLASS(IConfig)
    [[nodiscard]] bool IsDirty() const { return dirty_; }

protected:
    void SetDirty(bool dirty);


    bool dirty_ = false;
};
} // namespace core
