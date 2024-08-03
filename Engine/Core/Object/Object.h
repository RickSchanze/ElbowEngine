/**
 * @file Object.h
 * @author Echo 
 * @Date 2024/4/1
 * @brief 
 */

#pragma once

#include "CoreDef.h"

#include <iostream>

enum EObjectFlag
{
    EOF_IsGameObject,   // 在游戏世界运行
    EOF_IsComponent,    // 这个Object是一个Component
    EOF_IsWindow,       // 这个是一个窗口对象
};

class Object
{
    RTTR_ENABLE()
    RTTR_REGISTRATION_FRIEND

    friend class ObjectCreateHelper;

public:
    Object() : Object(EOF_IsGameObject) {}

    explicit Object(const EObjectFlag flag) : flag_(flag) {}

    virtual ~Object();

    /**
     * 获取反射类型
     * @return rttr::typr
     */
    rttr::type GetType() const { return get_type(); }

public:
    /**
     * 获取对象名字
     * @return
     */
    String GetName() const { return name_; }

    /**
     * 获取对象的字符串表示
     * @return
     */
    virtual String ToString() const;

    /**
     * 获取对象ID
     * @return
     */
    int32_t GetID() const { return id_; }

    /**
     * 设置对象的名字
     * @param name
     */
    void SetName(const String& name);

    /**
     * 对象是否还有效
     * @return
     */
    virtual bool IsValid() const;

    bool IsComponent() const { return flag_ == EOF_IsComponent; }

    bool IsGameObject() const { return flag_ == EOF_IsGameObject; }

    // TODO: 位操作
    EObjectFlag GetObjectFlag() const { return flag_; }

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

    bool IsImplemented(const Type& other_type) const
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
