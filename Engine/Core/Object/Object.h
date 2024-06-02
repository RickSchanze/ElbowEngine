/**
 * @file Object.h
 * @author Echo 
 * @Date 2024/4/1
 * @brief 
 */

#pragma once

#include "CoreDef.h"

enum EObjectFlag {
    EOF_IsGameObject,   // 在游戏世界运行
    EOF_IsComponent,    // 这个Object是一个Component
    EOF_IsWindow,       // 这个是一个窗口对象
};

class Object {
    RTTR_ENABLE()
    RTTR_REGISTRATION_FRIEND

    friend class ObjectCreateHelper;

public:
    Object() : Object(EOF_IsGameObject) {}
    Object(const EObjectFlag InFlag) : mFlag(InFlag) {}

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
    String GetName() const { return mName; }

    /**
     * 获取对象的字符串表示
     * @return
     */
    virtual String ToString() const;

    /**
     * 获取对象ID
     * @return
     */
    uint32 GetID() const { return mID; }

    /**
     * 设置对象的名字
     * @param Name
     */
    void SetName(String Name) { mName = std::move(Name); }

    /**
     * 对象是否还有效
     * @return
     */
    virtual bool IsValid() const;

    EObjectFlag GetObjectFlag() const { return mFlag; }

protected:
    String mName;                // 对象名字
    uint32 mID        = -1;      // 对象ID
    bool   mIsGarbage = false;   // 是否是垃圾对象

    const EObjectFlag mFlag;
};

template<typename T>
concept IsObject = std::is_base_of_v<Object, T>;
