/**
 * @file Object.h
 * @author Echo 
 * @Date 2024/4/1
 * @brief 
 */

#pragma once

#include "CoreDef.h"
#include "Serialization/ISerializer.h"

#include <iostream>

enum EObjectFlag
{
    EOF_IsGameObject,   // 在游戏世界运行
    EOF_IsComponent,    // 这个Object是一个Component
    EOF_IsWindow,       // 这个是一个窗口对象
};

class Object : public ISerializer
{
    RTTR_ENABLE()
    RTTR_REGISTRATION_FRIEND

    friend class ObjectCreateHelper;

public:
    typedef Object ThisClass;
    Object() : Object(EOF_IsGameObject) {}

    explicit Object(const EObjectFlag flag) : flag_(flag) {}

    ~Object() override;

    /**
     * 获取反射类型
     * @return rttr::typr
     */
    [[nodiscard]] Type GetType() const { return get_type(); }

public:
    /**
     * 获取对象名字
     * @return
     */
    [[nodiscard]] String GetName() const { return name_; }

    /**
     * 获取对象的字符串表示
     * @return
     */
    [[nodiscard]] virtual String ToString() const;

    /**
     * 获取对象ID
     * @return
     */
    [[nodiscard]] int32_t GetID() const { return id_; }

    /**
     * 设置对象的名字
     * @param name
     */
    void SetName(const String& name);

    /**
     * 对象是否还有效
     * @return
     */
    [[nodiscard]] virtual bool IsValid() const;

    [[nodiscard]] bool IsComponent() const { return flag_ == EOF_IsComponent; }

    [[nodiscard]] bool IsGameObject() const { return flag_ == EOF_IsGameObject; }

    // TODO: 位操作
    [[nodiscard]] EObjectFlag GetObjectFlag() const { return flag_; }

    const AnsiString& GetCachedAnsiString();

    template<typename T>
    bool IsImplemented()
    {
        Type other_type = rttr::type::get<T>();
        if (other_type)
        {
            return IsImplemented(other_type);
        }
        return rttr::rttr_cast<T*>(this) != nullptr;
    }

    [[nodiscard]] bool IsImplemented(const Type& other_type) const
    {
        rttr::type this_type = GetType();
        bool       s         = this_type.is_derived_from(other_type);
        return s;
    }

    template<typename T>
    T* As()
    {
        T* t = rttr::rttr_cast<T*>(this);
        if (t != nullptr)
        {
            return t;
        }
        return dynamic_cast<T*>(this);
    }

#if REGION(序列化)
    void Serialize(Archive& ar) override;
#endif

protected:
    String  name_;                 // 对象名字
    int32_t id_         = 0;       // 对象ID
    bool    is_garbage_ = false;   // 是否是垃圾对象

    const EObjectFlag flag_;

private:
    AnsiString cached_ansi_string_;   // 缓存的Ansi字符串 用于ImGui绘制
};

template<typename T>
concept IsObject = std::is_base_of_v<Object, T>;
